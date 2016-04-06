#ifndef SOURCE_CLIENT_DIRECTORYLISTENER_H_
#define SOURCE_CLIENT_DIRECTORYLISTENER_H_

#include <utilities/include/shared_queue.hpp>
#include <utilities/include/exceptions.hpp>
#include <settings.hpp>

#include <Windows.h>
#include <ostream>
#include <functional>
#include <memory>

#define FIRST_FILTER FILE_NOTIFY_CHANGE_FILE_NAME|\
		FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE

#define SECOND_FILTER FILE_NOTIFY_CHANGE_DIR_NAME|\
		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SECURITY

//#define FILTERS FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|\
//		FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|\
//		FILE_NOTIFY_CHANGE_SECURITY

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
	DWORD filters;

	/**
	 * L'oggetto è copiabile.
	 * @param old old copy
	 */
	change_entity(const change_entity&old) = default;
	change_entity(std::shared_ptr<char>&, FILE_NOTIFY_INFORMATION*, DWORD);

	FILE_NOTIFY_INFORMATION& operator*();
	FILE_NOTIFY_INFORMATION* operator->();

	const FILE_NOTIFY_INFORMATION& operator*() const;
	const FILE_NOTIFY_INFORMATION* operator->() const;
};

std::wostream& operator<<(std::wostream&, const change_entity);

/**
 * Questa classe serve a monitorare i cambiamenti effettuati in
 * una cartella in un file system qualsiasi.
 */
class directory_listener {

	/**
	 * Risorsa di sistema riferita alla cartella.
	 */
	HANDLE dir;
	volatile bool running;

public:
	directory_listener(const wchar_t*);

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
		//FIXME: RIVEDERE!!! Ho vatto modifiche pesanti e non ho il coraggio di testarle!
		if (running)
			return;
		running = true;

		// Eventi per la sincronizzazione
		HANDLE hEvents[2] = { 0 };

		hEvents[0] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		if (!hEvents[0])
			throw utilities::io_exception();

		utilities::on_return<>([hEvents]() {
			CloseHandle(hEvents[0]);
		});

		hEvents[1] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		if (!hEvents[1])
			throw utilities::io_exception();

		utilities::on_return<>([hEvents]() {
			CloseHandle(hEvents[1]);
		});

		// BENE! Ora gli oggetti OVERLAPPED
		OVERLAPPED ovr[2] = { { 0 }, { 0 } };

		// Setto gli eventi...
		ovr[0].hEvent = hEvents[0];
		ovr[1].hEvent = hEvents[1];

		// Chiamo subito le due funzioni
		std::shared_ptr<char> buffs[2] { std::shared_ptr<char>(
				new char[NOTIF_INFO_BUFF_LENGHT]), std::shared_ptr<char>(
				new char[NOTIF_INFO_BUFF_LENGHT]) };
		while (running) {

			// Faccio partire entrambe le richieste
			if (ReadDirectoryChangesW(dir, static_cast<LPVOID>(buffs[0].get()),
			NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE, FIRST_FILTER,
			NULL, ovr, NULL) == 0
					|| ReadDirectoryChangesW(dir,
							static_cast<LPVOID>(buffs[0].get()),
							NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE,
							SECOND_FILTER, NULL, ovr + 1, NULL) == 0)
				throw utilities::fs_exception();

			// Ora aspetto sugli eventi!
			int id;
			DWORD garbage, flags;
			while (running) {
				// Aspetto
				switch (WaitForMultipleObjects(2, hEvents,
				FALSE, settings::inst().io_wait_ms.value)) {
				case WAIT_OBJECT_0:
					id = 0;
					flags = FIRST_FILTER;
					goto out_while;
				case WAIT_OBJECT_0 + 1:
					id = 1;
					flags = SECOND_FILTER;
					goto out_while;
				case WAIT_TIMEOUT:
					continue;
				default:
					throw utilities::io_exception();
				};
			}
out_while:
			if (!GetOverlappedResult(dir, ovr, &garbage, TRUE))
				throw utilities::io_exception();
			// Ci siamo! HO i dati! Ora devo solo scriverli!
			char*current = buffs[id].get();

#define buffFNI (reinterpret_cast<FILE_NOTIFY_INFORMATION*>(current))
			(_t->*func)(change_entity(buffs[id], buffFNI, flags));
			while (buffFNI->NextEntryOffset != 0) {
				current += buffFNI->NextEntryOffset;
				(_t->*func)(change_entity(buffs[id], buffFNI, flags));
			}
#undef buffFNI
			// Perfetto, ora genero un nuovo buffer
			buffs[id].reset(new char[NOTIF_INFO_BUFF_LENGHT]);
			//E faccio una nuova richiesta.
			if (ReadDirectoryChangesW(dir, static_cast<LPVOID>(buffs[id].get()),
			NOTIF_INFO_BUFF_LENGHT * sizeof(char), TRUE, flags,
			NULL, ovr + id, NULL) == 0)
				throw utilities::io_exception();

		}
	}

	void stop();

	~directory_listener();
}
;

}

#endif /* SOURCE_CLIENT_DIRECTORYLISTENER_H_ */
