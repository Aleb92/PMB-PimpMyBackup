#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#define SETTINGS_FILE_NAME "settings.ini"

#include <utilities/include/singleton.hpp>
#include <utilities/include/settings.hpp>


#include <unordered_map>
#include <string>
#include <locale>
#include <codecvt>

namespace client {


	SETTINGS_BEGIN(settings, SETTINGS_FILE_NAME);

	/**
	 * Nome del file di log
	 */
	SETTINGS_ENTRY(std::wstring, log_filename);
	SETTINGS_ENTRY(std::string, tree_filename);

	/**
	 * Cartella salvata
	 */
	SETTINGS_ENTRY(std::wstring, watched_dir);

	/**
	 * indirizzo ip (stringa) del server
	 */
	SETTINGS_ENTRY(std::wstring, server_host);
	SETTINGS_ENTRY(uint16_t, server_port);

	/**
	 * Questa variabile indica se sincronizzare o no (ovvero se scaricare di nuovo tutto dal server)
	 * La sincronizzazione deve avvvenire nel main prima che parta la pipeline, per evitare problemi
	 * quindi l'ordine nel main sarà
	 * sincronizza
	 * start
	 * stop
	 */
	SETTINGS_ENTRY(bool, synchronize);

	SETTINGS_END(settings);
}
#endif /* SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_ */
