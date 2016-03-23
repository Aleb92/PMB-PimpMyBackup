#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>
#include <utilities/include/socket.hpp>

#include <thread>

using namespace std;
using namespace utilities;
using namespace client;
using namespace server;

static std::wstring_convert<std::codecvt_utf8<wchar_t>> client::converter;

client::client() :
		dirListener(settings::inst().watched_dir) {
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

void client::sendAction(std::wstring& fileName, file_action& action,
		std::atomic<bool>&) {
	const pair<opcode, bool (client::*)(std::wstring&)> flag[] = {
			{ MOVE, move },
			{ CREATE, create },
			{ REMOVE, remove },
			{ CHMOD, chmod },
			{ VERSION, version }
	};

	// Questa deve finalmente inviare tutto quello che serve, in ordine.
	// Questa funzione quindi invia l'header generico,
	// Controlla che non ci siano stati problemi (e nel caso capisce dove)
	// e finisce col re-inserire (se necessario) in coda la /le operazioni
	// da ripetere e aggiornare il log.

	wstring realName = (action.op_code & MOVE) ? action.newName : fileName;
	file_action result = action;

	// Per prima cosa mi aggancio al server
	socket_stream sock(settings::inst().server_host, settings::inst().server_port);

	if(sock.send(settings::inst().username)!= settings::inst().username.value.length())
		goto retry;

	sock.send(settings::inst().password);
	sock.send(action.op_code);
	sock.send(action.timestamps);
	sock.send(fileName);

	for(auto f : flag){
		if(action.op_code & f.first){
			if(!(this->*f.second)(realName))
				goto retry;
		}//TODO: Mettere atomic<bool> anche qua (leggi sotto)
	}

	result.op_code &= ~WRITE;
	result.op_code ^= sock.recv<opcode>();
	if(result.op_code)
		action_merger::inst().add_change(fileName, action);

	//TODO: continuare con la WRITE e ricordare atomic<bool> per dirgli di fermarsi quando scrive
	//visto che e una operazione lunga

	return;

retry:
	action_merger::inst().add_change(fileName, action);
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
