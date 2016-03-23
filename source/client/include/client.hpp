
#ifndef SOURCE_CLIENT_INCLUDE_CLIENT_HPP_
#define SOURCE_CLIENT_INCLUDE_CLIENT_HPP_

#include <directorylistener.hpp>
#include <filesystem.hpp>
#include <log.hpp>
#include <thread>
#include <locale>
#include <codecvt>

namespace client {

class client {
	std::thread watcher, merger, dispatcher;

	friend class std::thread;

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	utilities::thread_pool thPool;

	filesystem fs;
	directory_listener dirListener;

	using shq = utilities::shared_queue<change_entity>;

	void merge();
	void dispatch();

	void sendAction(std::wstring&, file_action&, std::atomic<bool>&);

	bool move(std::wstring&);
	bool create(std::wstring&);
	bool remove(std::wstring&);
	bool chmod(std::wstring&);
	bool version(std::wstring&);
	bool write(std::wstring&);

public:
	client();

	void start();
	void stop();

	bool synchronize();

	~client();
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_CLIENT_HPP_ */
