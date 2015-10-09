/**
 * File per i socket. Contiene le classi di base per gestire i socket sia su windows che su linux
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>
#include <vector>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <winsock2.h>
# include <windows.h>
# include <utilities/include/atbegin.hpp>
# include <utilities/include/atend.hpp>

//WSAStartup call
int init_winsock(void);
int WSACleanupWrap();

//Alla fine ripuliamo
atEnd(WSACleanupWrap);

//All'inizio inizializziamo
atBegin(init_winsock);


// COMPATIBILITY define

# ifndef _serrno
#  define _serrno WSAGetLastError()
# endif

# define hValid(h) ((h) != INVALID_SOCKET)
# define close(A) closesocket(A)
# define inet_network(a) ntohl(inet_addr(a))

# define MSG_NOSIGNAL 0

typedef SOCKET socket_t;
typedef u_short in_port_t;
typedef int socklen_t;

#else
# include <cerrno>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <unistd.h>
# include <fcntl.h>

typedef int socket_t;

#define _serrno errno

#define hValid(h) ((h) >= 0)

#endif

/**
 * Porta di default, scelta da bonetto
 */
#define DEFAULT_PORT 6000


namespace utilities {



	/**
	 * Classe di base per i socket. E' semplicemente un contenitore per un descrittore di risorsa.
	 * Gestisce anche il cambio dalla modalit� blocking a quella non blocking
	 */
	class socket_base {

		bool blocking;

	protected:

	    socket_t handle;

		inline socket_base(socket_base&&mv) : handle(mv.handle), blocking(mv.blocking) {
		    mv.handle = -1;
		}

		/**
		 * Inizializzazione protetta della classe di base: solo i derivati possono usarla perch� non deve essere
		 * instanziabile questa classe.
		 * @param _hnd handle della risorsa
		 */
		inline socket_base(socket_t _hnd) : handle(_hnd), blocking(true) {
			if(!hValid(handle))
				throw errno;
		}
	public:

		/**
		 * Imposta la modalit� blocking della risorsa
		 * @param b
		 */
		inline void setBlocking(bool b = true);

		/**
		 * returns the mode of the socket.
		 * @return
		 */
		inline bool getBlocking() { return blocking; }

		/**
		 * not assignable
		 */
		const socket_base& operator=(const socket_base&) = delete;


		/**
		 * not copyable
		 */
		socket_base(const socket_base&) = delete;

		/**
		 * this just allocates system resources (e.g. calls "socket" function)
		 * @param af
		 * @param type
		 * @param protocol
		 */
		socket_base(int af, int type, int protocol);

		/**
		 * rilascia le risorse, se presenti. Lancia un'eccezione di tipo int in caso di errore.
		 */
		inline ~socket_base() { if(hValid(handle))
			if(close(handle)) throw _serrno; }
	};

	class socket_stream : public socket_base {
		/**
		 * permetto alla classe listener di accedere al costruttore protetto. Questa classe � in pratica l'unica
		 * che pu� instanziare in questo modo questa classe (e quindi anche la sua base)
		 */
		friend class socket_listener;
	protected:
		/**
		 * Inizializza la classe a partire da una risorsa di sistema gi� ottenuta.
		 * @param _h handle alla risorsa
		 * @param ip address
		 * @param port number
		 */
		inline socket_stream(socket_t _h, uint32_t ip, in_port_t port) : socket_base(_h), oppositeIp(ip), oppositePort(port) {}
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

		/**
		 * Inizializza un nuovo socket e si connette all'host:port desiterato.
		 * @param ip
		 * @param port
		 * @param af
		 * @param type
		 * @param protocol
		 */
		socket_stream(uint32_t ip, in_port_t port, int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);

		socket_stream(const char * ip, in_port_t port, int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);

		/**
		 * Invia un singolo oggetto di tipo T
		 * @param val
		 * @return The number of bytes sent or -1
		 */
		template<typename T>
		inline ssize_t send(const T val) {
			return ::send(handle, (const char*)&val, sizeof(T), MSG_NOSIGNAL);
		}

		/**
		 * Invia un array di oggetti di tipo T, deducendone la dimensione in automatico.
		 * @param buff
		 * @return
		 */
		template<typename T, size_t s>
		inline ssize_t send(const T (&buff)[s]) {
			return ::send(handle, (const char*)buff, sizeof(buff), MSG_NOSIGNAL);
		}

		// Questa � vuota
		template <typename T>
		inline ssize_t send(const T, size_t); // empty

		/**
		 * recives an array of T
		 * @param buff array
		 * @param N number of elements in the array
		 * @return
		 */
		template<typename T>
		inline ssize_t send(const T*buff, size_t N) {
			return ::send(handle, (const char*)buff, N*sizeof(T), MSG_NOSIGNAL);
		}

		template<typename T>
		inline ssize_t send(const std::vector<T> &v) {
			return ::send(handle, (const char*)&v[0], v.size()*sizeof(T), MSG_NOSIGNAL);
		}

		/**
		 * Riceve un dato di tipo T. Lancia un'eccezione se la ricezione non risulta possibile.
		 * (attenti quindi ad usarlo in caso di non blocking socket)
		 * @return the object received
		 */
		template<typename T>
		inline T recv() {
			T ret;
			if(::recv(handle, (char*)&ret, sizeof(T), MSG_NOSIGNAL) != sizeof(T)){
				int err = _serrno;
				throw err;
			}
			return ret;
		}

		template<typename T>
		inline ssize_t recv(const T); // Empty


		/**
		 * Riceve un array di dati
		 * @param buff
		 * @param N
		 * @return
		 */
		template<typename T>
		inline ssize_t recv(T*buff, size_t N) {
			return ::recv(handle, (char*)buff, N*sizeof(T), MSG_NOSIGNAL);
		}

		template<typename T, size_t N>
		inline ssize_t recv(T (&buff)[N]) {
			return ::recv(handle, (char*)buff, N*sizeof(T), MSG_NOSIGNAL);
		}

		template<typename T>
		inline ssize_t recv(std::vector<T> &v) {
		    return ::recv(handle, (char*)&v[0], v.size()*sizeof(T), MSG_NOSIGNAL);
		}
	};

	class socket_listener : public socket_base {
	public:
		socket_listener(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP, uint32_t ip = INADDR_ANY, in_port_t port = DEFAULT_PORT);
        socket_stream accept(int);
	};

} /* namespace utilities */
#endif /* SOCKET_H_ */
