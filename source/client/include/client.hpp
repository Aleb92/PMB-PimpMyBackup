
#ifndef SOURCE_CLIENT_INCLUDE_CLIENT_HPP_
#define SOURCE_CLIENT_INCLUDE_CLIENT_HPP_

#include <directorylistener.hpp>
#include <log.hpp>
#include <utilities/include/threadpool.hpp>

#include <thread>
#include <locale>
#include <codecvt>

namespace client {

class client {
	std::thread fileWatcher, dirWatcher, merger, dispatcher, tombstone;

	friend class std::thread;

	utilities::thread_pool thPool;

	directory_listener dirListener, fileListener;

	using shq = utilities::shared_queue<change_entity>;

	void merge();
	void dispatch();

	void sendAction(std::wstring&, file_action&, volatile bool&);

	void move(utilities::socket_stream&, std::wstring&);
	void create(utilities::socket_stream&, std::wstring&);
	void remove(utilities::socket_stream&, std::wstring&);
	void chmod(utilities::socket_stream&, std::wstring&);
	void moveDir(utilities::socket_stream&, std::wstring&);
	void version(utilities::socket_stream&, std::wstring&, volatile bool&);
	void write(utilities::socket_stream&, std::wstring&, volatile bool&);

public:
	client();

	client& operator=(client&&)= default;

	void start();
	void stop();

	~client();
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_CLIENT_HPP_ */
