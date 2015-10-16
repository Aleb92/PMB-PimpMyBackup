
#include <directorylistener.h>

#include <windows.h>

#include <utilities/include/fsutil.hpp>
#include <utilities/include/shared_queue.hpp>

#define FILTERS FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|\
		FILE_NOTIFY_CHANGE_SECURITY

#define NOTIF_INFO_BUFF_LENGHT 1024

using namespace utilities;
using namespace	client;

namespace client {

	directory_listener::directory_listener(const char* path) {
		dir = CreateFile(path, GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,
				0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

		if(dir == INVALID_HANDLE_VALUE){
			//MAX path lenght = 248 char and must be absolute
			if(createDirectoryRecursively(path)!=ERROR_SUCCESS){
				throw GetLastError();
			}
		}

	}

	void directory_listener::scan() {

		FILE_NOTIFY_INFORMATION buffFileNotifyInfo[NOTIF_INFO_BUFF_LENGHT];
		DWORD dwBytesReturned = 0;

		while(TRUE) // FIXME Trovare un modo per fermare il loop di scan della cartella
		{
			if(ReadDirectoryChangesW (dir, (LPVOID)&buffFileNotifyInfo,
							sizeof(buffFileNotifyInfo), TRUE, FILTERS,
							&dwBytesReturned, 0, 0) == 0)
			throw GetLastError();

			//TODO Da finire
			for(int i=0; i*sizeof(FILE_NOTIFY_INFORMATION)< dwBytesReturned; i++){
				shared_queue<FILE_NOTIFY_INFORMATION>::inst().enqueue(buffFileNotifyInfo[i]);
			}
		}
	}


	directory_listener::~directory_listener() {
		// TODO
		CloseHandle(dir);
	}

} /* namespace client */
