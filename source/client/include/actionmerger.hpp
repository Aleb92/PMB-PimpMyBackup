#ifndef SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_
#define SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_

#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <Windows.h>

namespace client {

inline server::opcode get_flag_bit(DWORD);

/**
 * Struttura per memorizzare lo stato attuale di una modifica al file
 * non ancora inviata al server.
 */
struct file_action {
	uint8_t op_code;///< bitmask delle operazioni da inviare
	FILETIME timestamps[8];///< timestamp di ognuna delle operazioni
	std::wstring newName;///< nuovo nome del file (in caso di MOVE)
	file_action& operator|=(const log_entry_header&);
	file_action& operator^=(const log_entry_header&);
};

/**
 * Classe per aggregare i cambiamenti. E' in pratica una hash table che
 * tiene traccia dei files.
 */
class action_merger : public utilities::singleton<action_merger> {
	using maptype = std::unordered_map<std::wstring, file_action>;
	maptype map;///< mappa per i cambiamenti
	action_merger() = default;
	friend class utilities::singleton<action_merger>;
	friend class client::log;
public:

	typedef maptype::iterator iterator;
	typedef maptype::const_iterator const_iterator;

	virtual ~action_merger() = default;

	/**
	 * Marca un nuovo file come modificato.
	 * @param Info sull'entita' modificata
	 */
	void add_change(const change_entity&);

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
