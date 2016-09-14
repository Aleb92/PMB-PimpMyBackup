/**
 * File per i socket. Contiene le classi di base per gestire i socket sia su windows che su linux
 */

#ifndef SOCKET_H_
#define SOCKET_H_


#include <stdint.h>
#include <vector>

#define BUFF_LENGHT 4096

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <winsock2.h>
# include <windows.h>
# include <utilities/include/atbegin.hpp>
# include <utilities/include/atend.hpp>

# define hValid(h) ((h) != INVALID_SOCKET)

# define inet_network(a) ntohl(inet_addr(a))

# define MSG_NOSIGNAL 0

typedef SOCKET socket_t;
typedef u_short in_port_t;
typedef int socklen_t;

#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <unistd.h>
# include <fcntl.h>

typedef int socket_t;


#define closesocket(A) close(A)

#define hValid(h) ((h) >= 0)

#endif


#include <utilities/include/debug.hpp>
#include <utilities/include/exceptions.hpp>

/**
 * Porta di default, scelta da bonetto
 */
#define DEFAULT_PORT 6000
#define DEFAULT_QUEUE_SIZE 5

namespace utilities {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

//WSAStartup call
int init_winsock(void);
int WSACleanupWrap();

//Alla fine ripuliamo
atEnd(WSACleanupWrap);

//All'inizio inizializziamo
atBegin(init_winsock);
#endif

/**
 * Classe di base per i socket. E' semplicemente un contenitore per un descrittore di risorsa.
 * Gestisce anche il cambio dalla modalità blocking a quella non blocking
 */
class socket_base {

	bool blocking;

protected:

	socket_t handle;

	inline socket_base(socket_base&&mv) :
			handle(mv.handle), blocking(mv.blocking) {
		LOGF;
		LOGD("MOVE");
		mv.handle = -1;
	}

	/**
	 * Inizializzazione protetta della classe di base: solo i derivati possono usarla perch� non deve essere
	 * instanziabile questa classe.
	 * @param _hnd handle della risorsa
	 */
	inline socket_base(socket_t _hnd) :
			handle(_hnd), blocking(true) {
		if (!hValid(handle))
			throw socket_exception(__LINE__, __func__, __FILE__);
	}
public:

	/**
	 * Imposta la modalità blocking della risorsa
	 */
	inline void setBlocking(bool b = true);

	/**
	 * Ritorna se il socket e blocking o no
	 * @return true se blocking
	 */
	inline bool getBlocking() {
		return blocking;
	}

	/**
	 * La classe socket_base non è assegnabile
	 */
	const socket_base& operator=(const socket_base&) = delete;

	/**
	 * La classe socket_base non è copiabile
	 */
	socket_base(const socket_base&) = delete;

	/**
	 * Alloca le risorse di sistema e chiama effettivamente la funzione "socket"
	 * @param Family
	 * @param Type
	 * @param Protocol
	 */
	socket_base(int af, int type, int protocol);

	/**
	 * Rilascia le risorse, se presenti. Lancia un'eccezione in caso di errore.
	 */
	virtual ~socket_base();

	enum SOCK_STATE {
		NOT_READY = 0,
		READ_READY = 1,
		WRITE_READY = 2,
		BOTH_READY = READ_READY | WRITE_READY
	};

	SOCK_STATE getState();
};

inline socket_base::SOCK_STATE operator|(socket_base::SOCK_STATE lh, socket_base::SOCK_STATE rh) {
	return static_cast<socket_base::SOCK_STATE>((uint8_t)lh | rh);
}

/**
 * Classe socket dedicata ai socket di tipo stream (TCP)
 */
class socket_stream: public socket_base {
	/**
	 * permetto alla classe listener di accedere al costruttore protetto. Questa classe � in pratica l'unica
	 * che può instanziare in questo modo questa classe (e quindi anche la sua base)
	 */
	friend class socket_listener;
protected:
	/**
	 * Inizializza la classe a partire da una risorsa di sistema gi� ottenuta tramite il costruttore della
	 * classe base @link socket_base.
	 * @param _h handle alla risorsa
	 * @param ip address
	 * @param port number
	 */
	inline socket_stream(socket_t _h, uint32_t ip, in_port_t port) :
			socket_base(_h), oppositeIp(ip), oppositePort(port) {
			LOGF;
	}
public:
	/**
	 * Client ip address in formato binatio (host)
	 */
	const uint32_t oppositeIp;

	/**
	 * Client port in forato binario (host)
	 */
	const in_port_t oppositePort;

	const socket_stream& operator=(const socket_stream&) = delete;

	socket_stream(socket_stream&&);
	virtual ~socket_stream();

	/**
	 * Inizializza un nuovo socket e si connette all'host:port desiterato.
	 * @param ip
	 * @param port
	 * @param af
	 * @param type
	 * @param protocol
	 */
	socket_stream(uint32_t ip, in_port_t port, int af = AF_INET, int type =
	SOCK_STREAM, int protocol = IPPROTO_TCP);

	socket_stream(const char * ip, in_port_t port, int af = AF_INET, int type =
	SOCK_STREAM, int protocol = IPPROTO_TCP);

	inline socket_stream(const std::string ip, in_port_t port, int af = AF_INET,
			int type = SOCK_STREAM, int protocol = IPPROTO_TCP) :
			socket_stream(ip.c_str(), port, af, type, protocol) {
	}

	inline void setCoalesce(bool coalesce) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		DWORD
#else
		int
#endif
			flag = !coalesce;
		
		if(setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&flag), sizeof( flag ) ))
			throw socket_exception(__LINE__, __func__, __FILE__);		
	}
	
	/**
	 * Invia un singolo oggetto di tipo T
	 * @param Valore da mandare
	 * @return The number of bytes sent or -1
	 */
	template<typename T>
	void send(const T val) {
		LOGF;
		LOGD("di: " << typeid(T).name());

		if(::send(handle, (const char*) &val, sizeof(T), MSG_NOSIGNAL) != sizeof(T))
			throw socket_exception(__LINE__, __func__, __FILE__);
	}

	/**
	 * Invia un array di oggetti di tipo T, deducendone la dimensione in automatico.
	 * @param buff
	 */
	template<typename T, size_t s>
	void send(const T (&buff)[s]) {
		if(::send(handle, (const char*) buff, sizeof(buff), MSG_NOSIGNAL) != sizeof(buff))
			throw socket_exception(__LINE__, __func__, __FILE__);
	}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

	template<size_t s>
	void send(const FILETIME(&val)[s]) {
		LOGF;
		//FIXME e' giusto l'ordine?
		for(unsigned int i = 0; i < s; i++){
			LOGD(val[i]);
			send<uint32_t>((uint32_t)val[i].dwHighDateTime);
			send<uint32_t>((uint32_t)val[i].dwLowDateTime);
		}
	}

	template<size_t N>
	void recv(FILETIME (&buff)[N]) {
		for(unsigned int i = 0; i < N; i++){
			buff[i].dwHighDateTime = recv<uint32_t>();
			buff[i].dwLowDateTime = recv<uint32_t>();
		}
	}

#endif

	/**
	 * recives an array of T
	 * @param buff array
	 * @param N number of elements in the array
	 * @return
	 */
	template<typename T>
	void send(const T*buff, size_t N) {
		LOGF;
		size_t n;
		if((n = ::send(handle, (const char*) buff, N * sizeof(T), MSG_NOSIGNAL)) != N)
			throw socket_exception(__LINE__, __func__, __FILE__);
		LOGD("Invio contenuto buffer..." << " size: " << n);
	}

	/**
	 * Riceve un dato di tipo T. Lancia un'eccezione se la ricezione non risulta possibile.
	 * (attenti quindi ad usarlo in caso di non blocking socket)
	 * @return the object received
	 */
	template<typename T>
	T recv() {
		LOGF;
		LOGD("di: " << typeid(T).name());
		T ret;
		if (::recv(handle, (char*) &ret, sizeof(T), MSG_NOSIGNAL)
				!= sizeof(T)) {
			throw socket_exception(__LINE__, __func__, __FILE__);
		}

		LOGD("recv returned value = "<< ret);
		return ret;
	}

	template<typename T>
	T recv(size_t s);

	template<typename T>
	inline ssize_t recv(const T); // Empty

	/**
	 * Riceve un array di dati
	 * @param buff
	 * @param N
	 * @return
	 */
	template<typename T>
	ssize_t recv(T* buff, size_t N) {
		LOGF;
		LOGD("di: " << typeid(T).name());
		LOGD("size = " << N);
		ssize_t ret = ::recv(handle, static_cast<char*>(buff), N * sizeof(T), MSG_NOSIGNAL);
		LOGD(ret);
		if(ret < 0)
			throw socket_exception(__LINE__, __func__, __FILE__);
		return ret;
	}

	template<typename T, size_t N>
	inline ssize_t recv(T (&buff)[N]) {
		return recv(buff, N);
	}

};

// Qui dichiaro le specializzazioni template complete di size e recv
template<>
void socket_stream::send<uint16_t>(const uint16_t val) ;

template<>
void socket_stream::send<uint32_t>(const uint32_t val);

template<>
void socket_stream::send<int16_t>(const int16_t val) ;

template<>
void socket_stream::send<int32_t>(const int32_t val) ;

template<>
void socket_stream::send<int64_t>(const int64_t val) ;

template<>
void socket_stream::send<std::string&>(std::string& str) ;

template<>
void socket_stream::send<std::wstring&>(std::wstring& str) ;

template<>
uint16_t socket_stream::recv<uint16_t>();

template<>
uint32_t socket_stream::recv<uint32_t>();

template<>
int16_t socket_stream::recv<int16_t>();

template<>
int32_t socket_stream::recv<int32_t>();

template<>
int64_t socket_stream::recv<int64_t>();

template<>
std::string socket_stream::recv<std::string>();

template<>
std::string socket_stream::recv<std::string>(size_t);

/**
 * Socket dedicato solo ad accettare connesioni e a generare dei nuovi socket_stream (solitamente
 * usato lato server...)
 */
class socket_listener: public socket_base {
public:
	socket_listener(int af = AF_INET, int type = SOCK_STREAM, int protocol =
	IPPROTO_TCP, uint32_t ip = INADDR_ANY, in_port_t port = DEFAULT_PORT,
			int q_size = DEFAULT_QUEUE_SIZE);
	socket_stream accept();
};

} /* namespace utilities */
#endif /* SOCKET_H_ */
