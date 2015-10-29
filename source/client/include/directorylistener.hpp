

#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

#include <utilities/include/shared_queue.hpp>

#include <Windows.h>
#include <functional>
#include <memory>

#define FILTERS FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|\
		FILE_NOTIFY_CHANGE_SECURITY

#define K 1024
#define NOTIF_INFO_BUFF_LENGHT 64*K

namespace client {

	//FIXME: serve un nome più bello....
	class change_entity {
		std::shared_ptr<char> container;
		FILE_NOTIFY_INFORMATION*data;
	public:
		change_entity(std::shared_ptr<char>&, FILE_NOTIFY_INFORMATION*);
		FILE_NOTIFY_INFORMATION& operator*();
		FILE_NOTIFY_INFORMATION* operator->();
	};

	class directory_listener {

		HANDLE dir;

	public:
		directory_listener(const char*);

		template <void (*func)(const change_entity)>
			void scan(std::function<bool()> stopper) {

				DWORD dwBytesReturned = 0;

				while(stopper()) // FIXME Trovare un modo per fermare il loop di scan della cartella
				{
					//FIXME Decidere come gestoire la lunghezza enorme di questo vettore che sara
					//allocato nuovo ad ogni ciclo
					char buffFileNotifyInfo[NOTIF_INFO_BUFF_LENGHT];

					if(ReadDirectoryChangesW (dir, (LPVOID)&buffFileNotifyInfo,
									sizeof(buffFileNotifyInfo), TRUE, FILTERS,
									&dwBytesReturned, 0, 0) == 0)
					throw GetLastError();

					std::shared_ptr<char> whole(&buffFileNotifyInfo);

					char* current = buffFileNotifyInfo;

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))

					while(buffFNI->NextEntryOffset != 0){
						change_entity currentEnt(whole, current);
						func(currentEnt);
						current+=buffFNI->NextEntryOffset;
					}

#undef buffFNI
				}
			}

	template <typename T, T* _t, void (T::*func)(const change_entity)>
				void scan(std::function<bool()> stopper) {


					DWORD dwBytesReturned = 0;

					while(stopper()) // FIXME Trovare un modo per fermare il loop di scan della cartella
					{
						char buffFileNotifyInfo[NOTIF_INFO_BUFF_LENGHT];
						if(ReadDirectoryChangesW (dir, (LPVOID)&buffFileNotifyInfo,
										sizeof(buffFileNotifyInfo), TRUE, FILTERS,
										&dwBytesReturned, 0, 0) == 0)
						throw GetLastError();

						std::shared_ptr<char> whole(&buffFileNotifyInfo);
						char* current = buffFileNotifyInfo;

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))

						while(buffFNI->NextEntryOffset != 0){
							change_entity currentEnt(whole, current);
							(_t->*func)(currentEnt);
							current+=buffFNI->NextEntryOffset;
						}

#undef buffFNI
					}
				}

			~directory_listener();
		};

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */
