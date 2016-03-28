#include <utilities/include/socket.hpp>
#include <iostream> // Per std::cerr
#include <cstdlib>

using namespace std;

namespace utilities {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

/**
 * Questa funzione l'ho scopiazzata dalla guida windows
 */
int init_winsock(void) {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return err;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		std::cerr << "Impossibile trovare una versione adatta di Winsock.dll\n"
				<< std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}

int WSACleanupWrap() {
	return WSACleanup();
}

#endif

void socket_base::setBlocking(bool b) {
	if (b == blocking)
		return;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	u_long val = (int) !b;
	int code;
	if ((code = ioctlsocket(handle, FIONBIO, &val)))
		throw code;
#else
	int flags = fcntl(handle, F_GETFL, 0);
	if(flags < 0) throw _serrno;
	if(fcntl(handle, F_SETFL, flags | O_NONBLOCK)!=0) throw _serrno;
#endif
	blocking = b;
}
/**
 * Descr
 * @param af
 * @param type
 * @param protocol
 */
socket_base::socket_base(int af, int type, int protocol) :
		handle(::socket(af, type, protocol)), blocking(true) {
	// Controllo che tutto sia andato a buon fine, se no trow dell'eccezione
	if (!hValid(handle))
		throw _serrno;
}

////////////////////////////////////////////////////////////////////////////////

socket_stream::socket_stream(socket_stream&& ss) :
		socket_base((socket_base&&) ss), oppositeIp(ss.oppositeIp), oppositePort(
				ss.oppositePort) {
}

socket_stream::socket_stream(uint32_t ip, in_port_t port, int af, int type,
		int protocol) :
		socket_base(af, type, protocol), oppositeIp(ip), oppositePort(port) {
	struct sockaddr_in addr = { 0 };
	addr.sin_addr.s_addr = htonl(ip); // FIXME: questa serve?? devo davvero usare htonl?
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if (connect(handle, (struct sockaddr*) &addr, sizeof(addr)))
		throw _serrno;
}

socket_stream::socket_stream(const char * ip, in_port_t port, int af, int type,
		int protocol) :
		socket_base(af, type, protocol), oppositeIp(inet_network(ip)), oppositePort(
				port) {

	struct sockaddr_in addr = { 0 };
	addr.sin_addr.s_addr = htonl(oppositeIp);
	addr.sin_port = htons(oppositePort);
	addr.sin_family = AF_INET;

	if (connect(handle, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		cout << "Client: connet did not work" << endl;
		throw errno;
	}
}

////////////////////////////////////////////////////////////////////////////////

socket_listener::socket_listener(int af, int type, int protocol, uint32_t ip,
		in_port_t port, int q_size) :
		socket_base(af, type, protocol) {
	struct sockaddr_in addr = { 0 };
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr_in);

	addr.sin_family = af;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	if (::bind(handle, (struct sockaddr*) &addr, addr_len) < 0)
		throw _serrno;
	if (::listen(handle, q_size))
		throw _serrno;
}

socket_stream socket_listener::accept() {

	struct sockaddr_in client = { 0 };
	socklen_t len = sizeof(struct sockaddr_in);
	socket_t new_sock;

	if ((new_sock = ::accept(handle, (struct sockaddr *) &client,
			(socklen_t*) &len)) < 0)
		throw _serrno;

	return socket_stream(new_sock, client.sin_addr.s_addr, client.sin_port);
}

// specializzazioni template di send e recv

template<>
void socket_stream::send<uint16_t>(const uint16_t val) {
	uint16_t snd = htons(val);
	if(::send(handle, (const char*) &snd, sizeof(uint16_t), MSG_NOSIGNAL) != sizeof(val))
		throw errno;
}

template<>
void socket_stream::send<uint32_t>(const uint32_t val) {
	uint32_t snd = htons(val);
	if(::send(handle, (const char*) &snd, sizeof(uint32_t), MSG_NOSIGNAL) != sizeof(val))
		throw errno;
}

template<>
void socket_stream::send<int16_t>(const int16_t val) {
	int16_t snd = htons(val);
	if(::send(handle, (const char*) &snd, sizeof(int16_t), MSG_NOSIGNAL) != sizeof(val))
		throw errno;
}

template<>
void socket_stream::send<int32_t>(const int32_t val) {
	int32_t snd = htons(val);
	if(::send(handle, (const char*) &snd, sizeof(int32_t), MSG_NOSIGNAL) != sizeof(val))
		throw errno;
}

template<>
void socket_stream::send<const std::string&>(const std::string& str) {
	uint32_t size = str.length();
	send(size);
	send(str.c_str(), size);
}

template<>
uint16_t socket_stream::recv<uint16_t>() {
	uint16_t ret;
	if (::recv(handle, (char*) &ret, sizeof(uint16_t), MSG_NOSIGNAL)
			!= sizeof(uint16_t)) {
		int err = _serrno;
		throw err;
	}
	return ntohs(ret);
}

template<>
uint32_t socket_stream::recv<uint32_t>() {
	uint32_t ret;
	if (::recv(handle, (char*) &ret, sizeof(uint32_t), MSG_NOSIGNAL)
			!= sizeof(uint32_t)) {
		int err = _serrno;
		throw err;
	}
	return ntohl(ret);
}

template<>
int16_t socket_stream::recv<int16_t>() {
	int16_t ret;
	if (::recv(handle, (char*) &ret, sizeof(int16_t), MSG_NOSIGNAL)
			!= sizeof(int16_t)) {
		int err = _serrno;
		throw err;
	}
	return ntohs(ret);
}

template<>
int32_t socket_stream::recv<int32_t>() {
	int32_t ret;
	if (::recv(handle, (char*) &ret, sizeof(int32_t), MSG_NOSIGNAL)
			!= sizeof(int32_t)) {
		int err = _serrno;
		throw err;
	}
	return ntohl(ret);
}

template<>
std::string socket_stream::recv<std::string>() {

	size_t size = recv<uint32_t>();
	std::string ret(size, '\0');

	if (recv(&ret[0], size) != size)
		throw errno;

	return ret;
}

}/* namespace utilities */
