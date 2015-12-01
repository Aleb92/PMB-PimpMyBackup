
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

	thread watcher(&directory_listener::scan<shq, &shq::enqueue>, dirListener),
		   merger(&client::merge, this), dispatcher(&client::dispatch, this);

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
}




} /* namespace client */
