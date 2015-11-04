#include <windows.h>

#include <directorylistener.hpp>
#include <utilities/include/fsutil.hpp>

using namespace utilities;
using namespace client;

namespace client {

directory_listener::directory_listener(const wchar_t* path) {
	dir = CreateFile(path, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS, 0);

	if (dir == INVALID_HANDLE_VALUE) {
		//MAX path lenght = 248 char and must be absolute
		if (createDirectoryRecursively(path) != 0) {
			throw GetLastError();
		}
		dir = CreateFile(path, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0,
				OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	}

}

directory_listener::~directory_listener() {
	CloseHandle(dir);
}

} /* namespace client */
