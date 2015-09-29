/*
 * socket.h
 *
 *  Created on: Sep 29, 2015
 *      Author: s180678
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#include <windows.h>
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
typedef int socket_t;

#endif

#define DEFAULT_PORT 999

namespace utilities {


	class socket_base {
		socket_t handle;
	protected:
		socket_base(socket_t _hnd) : handle(_hnd) { }
	public:
		bool blocking;
		socket_base(const socket&&) = delete;
		socket_base(const socket&) = delete;
		socket_base(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);
		virtual ~socket_base();
	};

	class socket_stream {
		friend class socket_listener;
	protected:
		socket_stream(socket_t _h, uint32_t ip = INADDR_ANY, in_port_t port = DEFAULT_PORT);
	public:
		const uint32_t clientIp;
		const in_port_t clientPort;
		socket_stream(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP, uint32_t ip = INADDR_ANY, in_port_t port = DEFAULT_PORT);

		template<typename T>
		ssize_t send(T val);

		template<typename T>
		ssize_t send<T*>(const T*, size_t);

		template<typename T, size_t s>
		ssize_t send<T[s]>(const T buff[s]);

		template<typename T>
		T recv();

		template<typename T>
		ssize_t recv();

		template<typename T>
		ssize_t recv<T*>(const T*, size_t);

		template<typename T, size_t s>
		ssize_t send<T[s]>(const T buff[s]);


	};

	class socket_listener : public socket_base {
	public:
		socket_listener(int af = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP, uint32_t ip = INADDR_ANY, in_port_t port = DEFAULT_PORT);
		socket_stream&& accept();
	};

} /* namespace utilities */
#endif /* SOCKET_H_ */
