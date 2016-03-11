
#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#define SETTINGS_FILE_NAME "settings.ini"

#include <string>
#include <locale>
#include <codecvt>
#include <utilities/include/singleton.hpp>

namespace client {

/**
 * Classe per la gestione delle opzioni impostata come singleton per comodità.
 * All'apertura e alla chiusura del programma carica e salva le opzioni in un file.
 * Ogni modifica alle opzioni è quindi automaticamente serializzata durante la distruzione dell'oggetto.
 * Tutti i suoi membri sono quindi pubblici e modificabili.
 */
class settings : public utilities::singleton<settings> {
	friend class utilities::singleton<settings>;

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	settings();
	virtual ~settings();
public:
	/**
	 * Nome del file di log
	 */
	std::wstring log_filename;

	// FIXME: ?!?
	std::string tree_filename;

	/**
	 * Cartella salvata
	 */
	std::wstring watched_dir;

	/**
	 * indirizzo ip (stringa) del server
	 */
	std::wstring server_host;
	uint16_t server_port;

	/**
	 * Questa variabile indica se sincronizzare o no (ovvero se scaricare di nuovo tutto dal server)
	 * La sincronizzazione deve avvvenire nel main prima che parta la pipeline, per evitare problemi
	 * quindi l'ordine nel main sarà
	 * sincronizza
	 * start
	 * stop
	 */
	bool synchronize;

	//TODO aggiungere altro

};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_ */
