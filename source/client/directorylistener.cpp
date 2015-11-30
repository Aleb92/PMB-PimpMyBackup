
#include <directorylistener.hpp>
#include <utilities/include/fsutil.hpp>
#include <thread>
#include <windows.h>

#include <functional>

using namespace utilities;
using namespace client;

namespace client {

directory_listener::directory_listener(const wchar_t* path) : running(false) {
	dir = CreateFileW(path, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS, 0);

	if (dir == INVALID_HANDLE_VALUE) {
		//MAX path lenght = 248 char and must be absolute
		if (createDirectoryRecursively(path) != 0) {
			throw GetLastError();
		}
		dir = CreateFileW(path, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0,
				OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	}

}

directory_listener::~directory_listener() {
	CloseHandle(dir);
}

void directory_listener::scan(std::function<void(const change_entity)> func) {
	if(running)
		return; // TODO: eccezione?
	DWORD dwBytesReturned = 0;
	while (running)
	{
		//FIXME: Credo ci sia una corsa critica quì
		char *current = new char[NOTIF_INFO_BUFF_LENGHT];

		if (ReadDirectoryChangesW(dir, (LPVOID) current,
		NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE, FILTERS,
				&dwBytesReturned, NULL, NULL) == 0)
			throw GetLastError();

		std::shared_ptr<char> whole(current);

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))
		func(change_entity(whole, buffFNI));
		while (buffFNI->NextEntryOffset != 0) {
			current += buffFNI->NextEntryOffset;
			func(change_entity(whole, buffFNI));
		}
#undef buffFNI
	}
}

void directory_listener::stop() {
	if(running) {
		running = false;
		// fixme:Questo probabilmente elimina (nella pratica) il problema delle corse critiche
		// però... NO BIEN per la parte teorica che rimane dipendente dallo schedulatore
		std::this_thread::yield();
		// Interrompi l'attesa!
		CancelIoEx(dir, NULL);
	}
}

} /* namespace client */
