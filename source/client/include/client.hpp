
#ifndef SOURCE_CLIENT_INCLUDE_CLIENT_HPP_
#define SOURCE_CLIENT_INCLUDE_CLIENT_HPP_

#include <directorylistener.hpp>
#include <filesystem.hpp>
#include <log.hpp>
#include <thread>

namespace client {

class client {
	std::thread watcher, merger, dispatcher;

	friend class std::thread;

	utilities::thread_pool thPool;

	filesystem fs;
	directory_listener dirListener;

	using shq = utilities::shared_queue<change_entity>;

	void merge();
	void dispatch();

	void sendAction(std::wstring&, file_action&, std::atomic<bool>&);




public:
	client();

	void start();
	void stop();

	bool synchronize();

	~client();
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_CLIENT_HPP_ */
