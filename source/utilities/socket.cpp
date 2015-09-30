#include "socket.h"

using namespace utilities;
using namespace std;

void socket_base::setBlocking(bool b) {
	if(b == blocking)
		return;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	u_long val = (int)!b;
	int code;
	if((code = ioctlsocket(handle, FIONBIO, &val)))
		throw code;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags < 0) throw errno;
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK)!=0) throw errno;
#endif
    blocking = b;
}
/**
 * Descr
 * @param af
 * @param type
 * @param protocol
 */
socket_base::socket_base(int af, int type, int protocol):
		handle(::socket(af, type, protocol)), blocking(true){
	// Controllo che tutto sia andato a buon fine, se no trow dell'eccezione
	if(handle < 0)
		throw errno;
}

////////////////////////////////////////////////////////////////////////////////

socket_stream::socket_stream(socket_stream&& ss) :
        socket_base((socket_base&&)ss), clientIp(ss.clientIp), clientPort(ss.clientPort) { }

////////////////////////////////////////////////////////////////////////////////

socket_listener::socket_listener(int af, int type, int protocol, uint32_t ip,
                                    in_port_t port){
	struct sockaddr_in addr = {0};
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr_in);

	addr.sin_family = af;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	if(bind(handle, (struct sockaddr*)&addr, addr_len) < 0){
		throw errno;
}

socket_stream&& socket_listener::accept(int q_size){

    struct sockaddr_in client;
    socklen_t len;
    socket_t new_sock;

    listen(handle , q_size);
    s = sizeof(struct sockaddr_in);
    if((new_sock=::accept(handle, (struct sockaddr *)&client, (socklen_t*)&len))<0)
        throw errno;

    return move(sock_stream(new_sock, client.sin_addr.s_addr, client.sin_port));
}

/* namespace utilities */
