#ifndef SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_
#define SOURCE_CLIENT_INCLUDE_ACTIONMERGER_HPP_

#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <mutex>
#include <Windows.h>

namespace client {

inline server::opcode get_flag_bit(DWORD);

/**
 * Struttura per memorizzare lo stato attuale di una modifica al file
 * non ancora inviata al server.
 */

//TODO: Implementare move constructor per velocità?
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
	maptype::iterator it;
	std::mutex lock;
	std::condition_variable cv;

	action_merger(size_t estimatedFileNum = 100);

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
	 * Rimuove un elemento dalla mappa e lo restituisce
	 */
	bool remove(std::wstring&, file_action&);
};

}
#endif
