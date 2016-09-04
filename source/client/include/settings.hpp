#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#define SETTINGS_FILE_NAME "C:\\Users\\mrcmn\\workspace\\PMB\\client_settings.ini"

#include <utilities/include/socket.hpp>
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

	SETTINGS_ENTRY(int, max_waiting_time);
	SETTINGS_ENTRY(std::string, username);
	SETTINGS_ENTRY(std::string, password);

	/**
	 * Nome della pipe per comunicare con la GUI
	 */
	SETTINGS_ENTRY(std::string, pipe_name);

	/**
	 * Cartella salvata
	 */
	SETTINGS_ENTRY(std::wstring, watched_dir);

	/**
	 * Cartella temporanea
	 */
	SETTINGS_ENTRY(std::wstring, temp_dir);

	/**
	 * indirizzo ip (stringa) del server
	 */
	SETTINGS_ENTRY(std::string, server_host);
	SETTINGS_ENTRY(in_port_t, server_port);

	/**
	 * Questa variabile indica se sincronizzare o no (ovvero se scaricare di nuovo tutto dal server)
	 * La sincronizzazione deve avvvenire nel main prima che parta la pipeline, per evitare problemi
	 * quindi l'ordine nel main sarà
	 * sincronizza
	 * start
	 * stop
	 */
	//SETTINGS_ENTRY(bool, synchronize);

	SETTINGS_END(settings);
}
#endif /* SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_ */
