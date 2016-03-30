#ifndef SOURCE_SERVER_INCLUDE_SERVERMAIN_HPP_
#define SOURCE_SERVER_INCLUDE_SERVERMAIN_HPP_

#include <utilities/include/socket.hpp>
#include <utilities/include/threadpool.hpp>
#include <settings.hpp>

#include <sqlite3.h>

class server_main {
	utilities::socket_listener listener;
	utilities::thread_pool tPool;



	void work(utilities::socket_stream, sqlite3 *, volatile bool&);
public:
	server_main();
	void start();
	void stop();
	~server_main();
};

#endif /* SOURCE_SERVER_INCLUDE_SERVERMAIN_HPP_ */
