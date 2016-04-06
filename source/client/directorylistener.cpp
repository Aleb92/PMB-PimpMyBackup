
#include <directorylistener.hpp>
#include <utilities/include/fsutil.hpp>
#include <utilities/include/exceptions.hpp>

#include <thread>
#include <windows.h>
#include <functional>

using namespace utilities;
using namespace client;
using namespace std;

namespace client {

directory_listener::directory_listener(const wchar_t* path) : running(false) {
	dir = CreateFileW(path, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS, 0);

	if (dir == INVALID_HANDLE_VALUE) {
		//MAX path lenght = 248 char and must be absolute
		if (createDirectoryRecursively(path) != 0) {
			throw fs_exception();
		}
		dir = CreateFileW(path, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0,
				OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	}

}

directory_listener::~directory_listener() {
	stop();
	CloseHandle(dir);
}

void directory_listener::stop() {
	if(running) {
		running = false;
		while(!lock.try_lock()){
			this_thread::yield();
			// Interrompi l'attesa!
			CancelIoEx(dir, NULL);
		}
		lock.unlock();
	}
}

} /* namespace client */
