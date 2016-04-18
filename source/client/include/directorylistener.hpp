#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

#include <utilities/include/shared_queue.hpp>
#include <utilities/include/exceptions.hpp>

#include <Windows.h>
#include <ostream>
#include <functional>
#include <memory>

#define K 1024
#define NOTIF_INFO_BUFF_LENGHT 8*K

namespace client {

/**
 * Smart pointer (custom) per un puntatore, parte di un chunk di memoria contigua
 * che espone la classe FILE_NOTIFY_INFORMATION.
 * Solo quando tutti i membri dell'array vengono distrutti, la memoria del
 * chunk intero viene liberata.
 */
class change_entity {
	/**
	 * Smart pointer per il chunk di memoria: non voglio certo gestire io
	 * la memoria o la copia!
	 */
	std::shared_ptr<char> container;
	/**
	 * Puntatore ai dati veri e propri
	 */
	FILE_NOTIFY_INFORMATION*data;
public:

	FILETIME time;
	DWORD flags;

	/**
	 * L'oggetto è copiabile.
	 * @param old old copy
	 */
	change_entity(const change_entity&old) = default;
	change_entity(std::shared_ptr<char>&, FILE_NOTIFY_INFORMATION*, DWORD flags);


	FILE_NOTIFY_INFORMATION& operator*();
	FILE_NOTIFY_INFORMATION* operator->();

	const FILE_NOTIFY_INFORMATION& operator*() const;
	const FILE_NOTIFY_INFORMATION* operator->() const;
};

std::wostream& operator<< (std::wostream&, const change_entity);

/**
 * Questa classe serve a monitorare i cambiamenti effettuati in
 * una cartella in un file system qualsiasi.
 */
class directory_listener {

	/**
	 * Risorsa di sistema riferita alla cartella.
	 */
	HANDLE dir;
	std::mutex lock;
	volatile bool running;
    DWORD flags;

public:
	directory_listener(const wchar_t*, DWORD);

	directory_listener& operator=(directory_listener&&);

	/**
	 * Questa funzione template prende in input una funzione di interruzione
	 * che segnala se serve ancora continuare a monitorare.
	 * Questa funzione inoltre richiama a ogni modifica la funzione passata
	 * come template. La funzione è un template per permettere un maggior
	 * grado di ottimizzazione!
	 * @param _t istanza della classe su cui chiamare il callback
	 */
	template<typename T, void (T::*func)(const change_entity)>
	void scan(T* _t) {
		try {
		if(running)
			return;
		running = true;
		DWORD dwBytesReturned = 0;

		std::lock_guard<std::mutex> guard (lock);

		while (running)
		{
			char *current = new char[NOTIF_INFO_BUFF_LENGHT];

			if (ReadDirectoryChangesW(dir, (LPVOID) current,
			NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE, flags,
					&dwBytesReturned, NULL, NULL) == 0)
				throw utilities::fs_exception();
			lock.unlock();

			std::shared_ptr<char> whole(current);

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))
			(_t->*func)(change_entity(whole, buffFNI, flags));
			while (buffFNI->NextEntryOffset != 0) {
				current += buffFNI->NextEntryOffset;
				(_t->*func)(change_entity(whole, buffFNI, flags));
			}

#undef buffFNI
			lock.lock();
		}
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	void stop();

	~directory_listener();
};

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */


