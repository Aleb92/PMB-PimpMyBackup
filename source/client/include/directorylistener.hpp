#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

#include <utilities/include/shared_queue.hpp>

#include <Windows.h>
#include <ostream>
#include <functional>
#include <memory>

#define FILTERS FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|\
		FILE_NOTIFY_CHANGE_SECURITY

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
	/**
	 * L'oggetto è copiabile.
	 * @param old copy
	 */
	change_entity(const change_entity&) = default;
	change_entity(std::shared_ptr<char>&, FILE_NOTIFY_INFORMATION*);

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

public:
	directory_listener(const wchar_t*);

	/**
	 * Questa funzione template prende in input una funzione di interruzione
	 * che segnala se serve ancora continuare a monitorare.
	 * Questa funzione inoltre richiama a ogni modifica la funzione passata
	 * come template. La funzione è un template per permettere un maggior
	 * grado di ottimizzazione!
	 * @param stopper
	 */
	template<typename T, void (T::*func)(const change_entity)>
	void scan(T* _t) {
		if(running)
			return; // TODO: eccezione?
		running = true;
		DWORD dwBytesReturned = 0;

		std::lock_guard<std::mutex> guard (lock);

		while (running)
		{
			char *current = new char[NOTIF_INFO_BUFF_LENGHT];

			if (ReadDirectoryChangesW(dir, (LPVOID) current,
			NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE, FILTERS,
					&dwBytesReturned, NULL, NULL) == 0)
				throw GetLastError();
			lock.unlock();

			std::shared_ptr<char> whole(current);

#define buffFNI ((FILE_NOTIFY_INFORMATION*)(current))
			(_t->*func)(change_entity(whole, buffFNI));
			while (buffFNI->NextEntryOffset != 0) {
				current += buffFNI->NextEntryOffset;
				(_t->*func)(change_entity(whole, buffFNI));
			}

#undef buffFNI
			lock.lock();
		}
	}

	void stop();

	~directory_listener();
};

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */
