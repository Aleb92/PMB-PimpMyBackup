#include <utilities/include/socket.hpp>

#include <client.hpp>
#include <directorylistener.hpp>
#include <settings.hpp>
#include <actionmerger.hpp>
#include <pipe.hpp>

#include <utilities/include/atend.hpp>
#include <utilities/include/exceptions.hpp>

#include <windows.h>
#include <thread>
#include <string>
#include <utility>

using namespace std;
using namespace utilities;
using namespace server;

namespace client {

#define FILE_FILTER FILE_NOTIFY_CHANGE_FILE_NAME|\
		FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE

#define DIR_FILTER FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SECURITY

client::client() :
		dirListener(settings::inst().watched_dir.value.c_str(), DIR_FILTER), fileListener(
				settings::inst().watched_dir.value.c_str(), FILE_FILTER) {
}

client::~client() {
}

void client::start() {
	dirWatcher = thread(&directory_listener::scan<shq, &shq::enqueue>,
			&dirListener, &shq::inst());
	fileWatcher = thread(&directory_listener::scan<shq, &shq::enqueue>,
			&fileListener, &shq::inst());
	merger = thread(&client::merge, this);
	dispatcher = thread(&client::dispatch, this);
	// Qui undertaker è pronto! TOMBSTONE PIPE DRIVER!!!
	tombstone = thread(&pipe::driver, &pipe::inst());
}

void client::merge() {
	try {
		change_entity che = shq::inst().dequeue();

		//Questo continua finche l'azione che non viene fuori e' 0 che significa chiusura
		while (che->Action) {

			if (get_flag_bit(che->Action, che.flags) != INVALID) {

				log::inst().issue(che);
				action_merger::inst().add_change(che);
			}
			che = shq::inst().dequeue();
		}
	} catch (const exception& e) {
		cout << e.what() << endl;
	}
}

void client::dispatch() {

	//Spawna thread e dialoga con il server tramite una condition variable
	//per avere le modifiche all'action merger

	try {
		wstring fileName;
		file_action newAction;

		while (action_merger::inst().remove(fileName, newAction)) {
			thPool.execute(sendAction, this, std::ref(fileName),
					std::ref(newAction));

			if (action_merger::inst().wait_time != 0)
				Sleep(action_merger::inst().wait_time);
		}
	} catch (const exception& e) {
		cout << e.what() << endl;
	}
}

void client::sendAction(std::wstring& fileName, file_action& action,
		volatile bool &run) {
	const pair<opcode, void (client::*)(socket_stream&, std::wstring&)> flag[] =
			{ { MOVE, move }, { CREATE, create }, { REMOVE, remove }, { CHMOD,
					chmod }, { MOVE_DIR, moveDir } };


	// Questa deve finalmente inviare tutto quello che serve, in ordine.
	// Questa funzione quindi invia l'header generico,
	// Controlla che non ci siano stati problemi (e nel caso capisce dove)
	// e finisce col re-inserire (se necessario) in coda la /le operazioni
	// da ripetere e aggiornare il log.

	wstring realName = (action.op_code & MOVE) ? action.newName : fileName;
	file_action result = action;

	wcout << L"sendAction:" << realName << endl << L"Connection..." << endl;

	result.op_code = 0;

	try {
		// Per prima cosa mi aggancio al server
		socket_stream sock(settings::inst().server_host,
				settings::inst().server_port);

		wcout << L"Connected" << endl << "Login...";

		sock.send(settings::inst().username.value);
		sock.send(settings::inst().password.value);

		if (!sock.recv<bool>())
			throw auth_exception();

		wcout << L"Login fatto!" << endl;

		sock.send(fileName);
		sock.send(action.op_code);
		sock.send(action.timestamps);

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


		if (action.op_code != 0) {
			wcout << L"Azione completata" << endl;
			action_merger::inst().add_change(fileName, action);
		}
		else
			wcout << L"Azione NON completata" << endl;

		action_merger::inst().wait_time = 0;

	} catch (base_exception& ex) { // ECCEZIONI
		if (dynamic_cast<socket_exception*>(&ex) != nullptr) {
			action_merger::inst().wait_time = min(
					settings::inst().max_waiting_time.value,
					(action_merger::inst().wait_time * 2) + 1);
		}
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
	fileListener.stop();

	// E aspettare che tutto sia effettivamento chiuso
	if (dirWatcher.joinable())
		dirWatcher.join();
	if (fileWatcher.joinable())
		fileWatcher.join();

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
}

void client::remove(socket_stream& sock, std::wstring& fileName) {
}

void client::chmod(socket_stream& sock, std::wstring& fileName) {

	uint32_t mods;
	if ((mods = GetFileAttributesW(fileName.c_str())) == INVALID_FILE_ATTRIBUTES)
		throw base_exception();
	sock.send(mods);
}

void client::moveDir(socket_stream& sock, wstring& fileName) {
	sock.send(fileName);
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
	sock.recv<bool>();
}

}
