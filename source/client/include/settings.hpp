
#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#define SETTINGS_FILE_NAME "settings.ini"

#include <string>
#include <locale>
#include <codecvt>
#include <utilities/include/singleton.hpp>

namespace client {

/**
 * Automazione per le impostazioni: carica e salva una variabile...
 * DA NON USARE PER DEL TESTO CON PIU' DI UNA RIGA.
 * Da usare insieme alla sua macro @see SETTINGS_ENTRY .
 * RENDIAMO GRAZIA a c++11 che con le sue nuove funzionalità
 * ha reso questo semplice da implementare
 */
template <typename T>
class settings_entry {
	const char*name;
	T value;
	std::iostream& inout;
public:
	settings_entry(const settings_entry<T> &) = delete; // no copy
	settings_entry<T>& operator=(const settings_entry<T>&) = delete; // no assign

	inline operator T() const noexcept { return value; }
	inline T operator= (const T& val){ return value = val; }

	settings_entry(const char*_name, settings_entry<std::ostream>& f, std::unordered_map<std::string, std::string>) : name(_name), inout(f) {
		std::string buff;
		if ( std::getline(inout, buff, '=') ) {
	    	// Questo è solo un controllo...
	    	if(buff == name) {
	    		// Questo è solo il comportamento di default..
	    		//Estraggo
	    		inout >> value;
	    		// Ok ora devo consumare il resto della riga..
	    		std::getline(inout, buff);
	    	}
	    		//FIXME: else exception?
		}
		// FIXME: else exception?
	}

	~settings_entry();
};

#define SETTINGS_ENTRY(type, name)

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
	~settings();
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
