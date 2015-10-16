

#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

#include <utilities/include/shared_queue.hpp>

#include <Windows.h>
#include <functional>

#define FILTERS FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|\
		FILE_NOTIFY_CHANGE_SECURITY

#define K 1024
#define NOTIF_INFO_BUFF_LENGHT 64*K

namespace client {

	class directory_listener {

		HANDLE dir;

	public:
		directory_listener(const char*);

		template <void (*func)(const FILE_NOTIFY_INFORMATION&)>
			void scan(std::function<bool()> stopper) {

				char buffFileNotifyInfo[NOTIF_INFO_BUFF_LENGHT];
				DWORD dwBytesReturned = 0;

				while(stopper()) // FIXME Trovare un modo per fermare il loop di scan della cartella
				{
					if(ReadDirectoryChangesW (dir, (LPVOID)&buffFileNotifyInfo,
									sizeof(buffFileNotifyInfo), TRUE, FILTERS,
									&dwBytesReturned, 0, 0) == 0)
					throw GetLastError();

					char* current = buffFileNotifyInfo;

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))

					while(buffFNI->NextEntryOffset != 0){
						func(*buffFNI);
						current+=buffFNI->NextEntryOffset;
					}

#undef buffFNI
				}
			}

	template <typename T, T* _t, void (T::*func)(const FILE_NOTIFY_INFORMATION&)>
				void scan(std::function<bool()> stopper) {

					char buffFileNotifyInfo[NOTIF_INFO_BUFF_LENGHT];
					DWORD dwBytesReturned = 0;

					while(stopper()) // FIXME Trovare un modo per fermare il loop di scan della cartella
					{
						if(ReadDirectoryChangesW (dir, (LPVOID)&buffFileNotifyInfo,
										sizeof(buffFileNotifyInfo), TRUE, FILTERS,
										&dwBytesReturned, 0, 0) == 0)
						throw GetLastError();

						char* current = buffFileNotifyInfo;

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))

						while(buffFNI->NextEntryOffset != 0){
							(_t->*func)(buffFNI);
							current+=buffFNI->NextEntryOffset;
						}

#undef buffFNI
					}
				}

			~directory_listener();
		};

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */
