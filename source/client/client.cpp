
#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>

#include <thread>

using namespace std;

namespace client {

client::client() : dirListener(settings::inst().watched_dir.c_str()) { }

client::~client() {
}

void client::start() {
	watcher = thread(&directory_listener::scan<shq, &shq::enqueue>, &dirListener, & shq::inst());
	merger = thread(&client::merge, this);
	dispatcher = thread(&client::dispatch, this);
}

void client::dispatch(){
	//TODO Spawna thread e dialoga con il server tramite una condition variable
	//per avere le modifiche all'action merger
}

void client::merge(){
	//TODO Implementare merge
}

void client::stop() {
	//TODO Implementare stop

	// E aspettare che tutto sia effettivamento chiuso
	if(watcher.joinable())
		watcher.join();
	if(merger.joinable())
		merger.join();
	if(dispatcher.joinable())
		dispatcher.join();
}

} /* namespace client */
