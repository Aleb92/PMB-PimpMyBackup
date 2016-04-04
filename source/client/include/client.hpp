
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
	std::thread watcher, merger, dispatcher;

	friend class std::thread;

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	utilities::thread_pool thPool;

	directory_listener dirListener;

	using shq = utilities::shared_queue<change_entity>;

	void merge();
	void dispatch();

	void sendAction(std::wstring&, file_action&, volatile bool&);

	void move(utilities::socket_stream&, std::wstring&);
	void create(utilities::socket_stream&, std::wstring&);
	void remove(utilities::socket_stream&, std::wstring&);
	void chmod(utilities::socket_stream&, std::wstring&);
	void version(utilities::socket_stream&, std::wstring&, volatile bool&);
	void write(utilities::socket_stream&, std::wstring&, volatile bool&);

public:
	client();

	void start();
	void stop();

	bool synchronize();

	~client();
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_CLIENT_HPP_ */
