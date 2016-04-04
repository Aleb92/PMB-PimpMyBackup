#include <utilities/include/socket.hpp>
#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>
#include <actionmerger.hpp>
#include <utilities/include/atend.hpp>
#include <utilities/include/exceptions.hpp>

#include <windows.h>
#include <thread>
#include <string>

using namespace std;
using namespace utilities;
using namespace server;

namespace client {
std::wstring_convert<std::codecvt_utf8<wchar_t>> client::converter;

client::client() :
		dirListener(settings::inst().watched_dir.value.c_str()) {
}

client::~client() {
}

bool client::synchronize() {
	//TODO
	return true;
}

void client::start() {
	watcher = thread(&directory_listener::scan<shq, &shq::enqueue>,
			&dirListener, &shq::inst());
	merger = thread(&client::merge, this);
	dispatcher = thread(&client::dispatch, this);
}

void client::merge() {

	change_entity che = shq::inst().dequeue();

	//Questo continua finche l'azione che non viene fuori e' 0 che significa chiusura
	while (che->Action) {
		// TODO: ora possiamo distinguere senza nessun problema tra dir e non dir
//		if (fs.isDir(che->FileName,
//				che->FileNameLength) && che->Action == FILE_ACTION_MODIFIED)
//			continue;

		log::inst().issue(che);
		action_merger::inst().add_change(che);
		che = shq::inst().dequeue();
	}
}

void client::dispatch() {

	//Spawna thread e dialoga con il server tramite una condition variable
	//per avere le modifiche all'action merger

	wstring fileName;
	file_action newAction;

	while (action_merger::inst().remove(fileName, newAction)) {
		thPool.execute(sendAction, this, std::ref(fileName),
				std::ref(newAction));
	}
}

void client::sendAction(std::wstring& fileName, file_action& action,
		volatile bool &run) {
	const pair<opcode, void (client::*)(socket_stream&, std::wstring&)> flag[] =
			{ { MOVE, move }, { CREATE, create }, { REMOVE, remove }, { CHMOD,
					chmod } };

	// Questa deve finalmente inviare tutto quello che serve, in ordine.
	// Questa funzione quindi invia l'header generico,
	// Controlla che non ci siano stati problemi (e nel caso capisce dove)
	// e finisce col re-inserire (se necessario) in coda la /le operazioni
	// da ripetere e aggiornare il log.

	wstring realName = (action.op_code & MOVE) ? action.newName : fileName;
	file_action result = action;
	result.op_code = 0;

	try {
		// Per prima cosa mi aggancio al server
		socket_stream sock(settings::inst().server_host,
				settings::inst().server_port);

		sock.send(settings::inst().username.value);
		sock.send(settings::inst().password.value);

		if (!sock.recv<bool>())
			throw auth_exception("username and/or password incorrect.");
		sock.send(action.op_code);
		sock.send(action.timestamps);
		sock.send(fileName);

		for (auto f : flag) {
			if ((action.op_code & f.first) && run) {
				(this->*f.second)(sock, realName);
				if (sock.recv<bool>()) {
					result.op_code |= f.first;
					action.op_code &= ~f.first;
				}
			}
		}

		if ((action.op_code & VERSION) && run) {
			this->version(sock, realName, run);
			if (sock.recv<bool>()) {
				result.op_code |= WRITE;
				action.op_code &= ~WRITE;
			}
		}

		if ((action.op_code & WRITE) && run) {
			this->write(sock, realName, run);
			if (sock.recv<bool>()) {
				result.op_code |= WRITE;
				action.op_code &= ~WRITE;
			}
		}

		if (action.op_code != 0)
			action_merger::inst().add_change(fileName, action);

	} catch (...) { // ECCEZIONI
		action_merger::inst().add_change(fileName, action);
	}

	if (result.op_code != 0)
		log::inst().finalize(result, fileName);
}

void client::stop() {

	FILE_NOTIFY_INFORMATION fni = { 0 };
	shared_ptr<char> shptr;
	const change_entity stop = change_entity(shptr, &fni, 0);

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

//////////////////////////////////////////////
/// 	PROTOCOL CLIENT IMPLEMENTATION     ///
//////////////////////////////////////////////

void client::move(socket_stream& sock, std::wstring& fileName) {
	sock.send(fileName);
}

void client::create(socket_stream& sock, std::wstring& fileName) {
	// TODO: utilizziamo qualche altro modo per fare questo
	// if (fs.isDir(fileName.c_str(), fileName.length()))
	//	sock.send('d');
	// else
	//	sock.send('f');

	sock.send(fileName);
}

void client::remove(socket_stream& sock, std::wstring& fileName) {
}

void client::chmod(socket_stream& sock, std::wstring& fileName) {
	// TODO: leggere i mod da file
	uint16_t mods = 0;//fs.get_file(fileName.c_str(), fileName.length()).mod;

	sock.send(mods);
}

void client::version(socket_stream& sock, std::wstring& fileName,
		volatile bool& run) {

	FILE* file = _wfopen(fileName.c_str(), L"wb");
	char buffer[BUFF_LENGHT] = { 0 };
	uint32_t n = 0;

	if (file == NULL)
		throw fs_exception();

	on_return<> ret([file]() {
		fclose(file);
	});

	uint32_t size = sock.recv<uint32_t>();

	while ((n < size) && run) {
		size_t i = sock.recv(buffer, BUFF_LENGHT);
		fwrite(buffer, i, 1, file);
		n += i;
	}

}

void client::write(socket_stream& sock, std::wstring& fileName,
		volatile bool& run) {

	char buffer[BUFF_LENGHT] = { 0 };
	FILE* file = _wfopen(fileName.c_str(), L"rb");
	if (file == NULL)
		throw fs_exception();

	on_return<> ret([file]() {
		fclose(file);
	});

	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	sock.send(size);

	while (feof(file) && run) {
		socket_base::SOCK_STATE state = sock.getState();
		if (state & socket_base::READ_READY) {
			if (sock.recv<bool>())
				return;
		}
		size_t readn = fread(buffer, BUFF_LENGHT, 1, file);
		sock.send(buffer, readn);
	}
}

}
