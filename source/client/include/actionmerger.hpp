#ifndef SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_
#define SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_

#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <Windows.h>

namespace client {

/**
 * Struttura per memorizzare lo stato attuale di una modifica al file
 * non ancora inviata al server.
 */
struct file_action {
	uint8_t op_code;///< bitmask delle operazioni da inviare
	FILETIME timestamps[8];///< timestamp di ognuna delle operazioni
	std::wstring newName;///< nuovo nome del file (in caso di MOVE)
};

/**
 * Classe per aggregare i cambiamenti. E' in pratica una hash table che
 * tiene traccia dei files.
 */
class action_merger {
	using maptype = std::unordered_map<std::wstring, file_action>;
	maptype map;///< mappa per i cambiamenti
public:

	typedef maptype::iterator iterator;
	typedef maptype::const_iterator const_iterator;

	action_merger() = default;
	virtual ~action_merger() = default;

	/**
	 * Marca un nuovo file come modificato.
	 * @param nome del file
	 * @param Azione notificata
	 * @param Timestamp
	 * @param newName nuovo nome (in caso di MOVE)
	 */
	void add_change(const wchar_t*, DWORD, const FILETIME&, const wchar_t * newName = nullptr);

	/**
	 * rimuove un elemento dalla mappa e restituisce il puntatore
	 * all'elemento successivo.
	 * @param iteratore dell'elemento attuale
	 * @return iteratore all'elemento successivo
	 */
	iterator remove(const_iterator);

	/**
	 * Segna un file come non modificato (lo rimuove dalla mappa)
	 * @param nome del file
	 */
	void remove(const wchar_t*);

	inline iterator begin() { return map.begin(); }
	inline const_iterator begin() const { return map.begin(); }
	inline iterator end() { return map.end(); }
	inline const_iterator end() const { return map.end(); }
};

}
#endif
