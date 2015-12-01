
#ifndef SOURCE_CLIENT_INCLUDE_CLIENT_HPP_
#define SOURCE_CLIENT_INCLUDE_CLIENT_HPP_

#include <directorylistener.hpp>
#include <filesystem.hpp>
#include <log.hpp>

namespace client {

class client {

	friend class std::thread;

	filesystem fs;
	directory_listener dirListener;

	using shq = utilities::shared_queue<change_entity, log, &log::issue<change_entity>, &log::close<change_entity>>;

	void merge();
	void dispatch();

public:
	client();

	void start();
	void stop();

	~client();
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_CLIENT_HPP_ */
