#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>
#include <utilities/include/socket.hpp>

#include <thread>

using namespace std;
using namespace utilities;
using namespace client;

client::client() :
		dirListener(settings::inst().watched_dir.c_str()) {
}

client::~client() {
}

void client::start() {
	watcher = thread(&directory_listener::scan<shq, &shq::enqueue>,
			&dirListener, &shq::inst());
	merger = thread(&client::merge, this);
	dispatcher = thread(&client::dispatch, this);
}

void client::merge() {

	change_entity che; // = shq::inst().dequeue();

	//Questo continua finche l'azione che non viene fuori e 0 che significa chiusura
	while ((che = shq::inst().dequeue())->Action) {

		if (fs.isDir(che->FileName,
				che->FileNameLength) && che->Action == FILE_ACTION_MODIFIED)
			continue;

		//TODO: Se serve controllare il checksum dei file quando ci arriva un file modified e se uguale
		// 		saltare

		log::inst().issue(che);
		action_merger::inst().add_change(che);
	}
}

void client::dispatch() {

	//Spawna thread e dialoga con il server tramite una condition variable
	//per avere le modifiche all'action merger

	wstring fileName;
	file_action newAction;

	while (action_merger::inst().remove(fileName, newAction)) {
		thPool.execute(sendAction, this, fileName, newAction);
	}
}

void sendAction(std::wstring&, file_action&, std::atomic<bool>&) {
	// Questa deve finalmente inviare tutto quello che serve, in ordine.
	// Questa funzione quindi invia l'header generico,
	// Controlla che non ci siano stati problemi (e nel caso capisce dove)
	// e finisce col re-inserire (se necessario) in coda la /le operazioni
	// da ripetere e aggiornare il log.

	// Per prima cosa mi aggancio al server
	socket_stream sock(settings::inst().server_host.c_str());

	// TODO: Autenticazione


}


void client::stop() {

	FILE_NOTIFY_INFORMATION fni = { 0 };
	shared_ptr<char> shptr;
	const change_entity stop = change_entity(shptr, &fni);

	dirListener.stop();

	// E aspettare che tutto sia effettivamento chiuso
	if (watcher.joinable())
		watcher.join();

	shq::inst().enqueue(stop);

	if (merger.joinable())
		merger.join();

	action_merger::inst().add_change(stop);

	thPool.stop();

	if (dispatcher.joinable())
		dispatcher.join();
}
