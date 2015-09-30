/*
 * socket.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: s180678
 */

#include "socket.h"

using namespace utilities;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
void socket_base::setBlocking(bool b) {
	if(b == blocking)
		return;
	u_long val = (int)!b;
	int code;
	if((code = ioctlsocket(handle, FIONBIO, &val)))
		throw code;
	blocking = b;
}
#else
// NON USARE setSockOpt, ma usare ioctl
void socket_base::setBlocking(bool b) {} // QUESTA LA FA BONETTO
#endif

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

socket_listener::socket_listener(int af, int type, int protocol, uint32_t ip, in_port_t port){
	struct sockaddr_in addr = {0};
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr_in);

	addr.sin_family = af;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	bind(handle, (struct sockaddr*)&addr, addr_len);
}

socket_stream&& accept(){

}

/* namespace utilities */
