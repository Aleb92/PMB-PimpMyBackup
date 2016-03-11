
#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>

#include <thread>

using namespace std;
using namespace utilities;

namespace client {

client::client() : dirListener(settings::inst().watched_dir.c_str()) { }

client::~client() {
}

void client::start() {
	watcher = thread(&directory_listener::scan<shq, &shq::enqueue>, &dirListener, & shq::inst());
	merger = thread(&client::merge, this);
	dispatcher = thread(&client::dispatch, this);
}

void client::merge(){

	change_entity che;// = shq::inst().dequeue();

	//Questo continua finche l'azione che non viene fuori e 0 che significa chiusura
	while((che = shq::inst().dequeue())->Action) {


		log::inst().issue(che);
		action_merger::inst().add_change(che);
	}

}

void client::dispatch(){
	//TODO Spawna thread e dialoga con il server tramite una condition variable
	//per avere le modifiche all'action merger
}

void client::stop() {

	FILE_NOTIFY_INFORMATION fni = {0};
	shared_ptr<char> shptr;
	const change_entity stop = change_entity(shptr, &fni);

	dirListener.stop();

	// E aspettare che tutto sia effettivamento chiuso
	if(watcher.joinable())
		watcher.join();

	shq::inst().enqueue(stop);

	if(merger.joinable())
		merger.join();

	//TODO: Finire di fare il dispatcher e vedere come terminarlo

	if(dispatcher.joinable())
		dispatcher.join();
}

} /* namespace client */
