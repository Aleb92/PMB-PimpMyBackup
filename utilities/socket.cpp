/*
 * socket.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: s180678
 */

#include "socket.h"

using namespace utilities;

/**
 * Descr
 * @param af
 * @param type
 * @param protocol
 */
socket_base::socket_base(int af, int type, int protocol):
		handle(::socket(af, type, protocol)), blocking(true){}

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
