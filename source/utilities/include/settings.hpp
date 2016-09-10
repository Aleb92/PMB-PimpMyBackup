#ifndef SOURCE_UTILITIES_INCLUDE_SETTINGS_HPP_
#define SOURCE_UTILITIES_INCLUDE_SETTINGS_HPP_

#include <utilities/include/singleton.hpp>
#include <utilities/include/fsutil.hpp>
#include <utilities/include/debug.hpp>
#include <utilities/include/atend.hpp>

#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cwchar>
#include <locale>
#include <codecvt>

namespace utilities  {
// FORWARD declaration
template<typename T>
class settings_entry;

class settings_io {
public:
	virtual ~settings_io();

	virtual operator std::unordered_map<std::string, std::stringstream>*()
			noexcept;

	virtual operator std::ofstream*() noexcept;
};

class settings_loader: public settings_io {
	std::unordered_map<std::string, std::stringstream> map;
public:
	settings_loader(const char*);

	virtual operator std::unordered_map<std::string, std::stringstream>*() noexcept;

	virtual ~settings_loader();
};

class settings_saver: public settings_io {
	std::ofstream out;
public:
	settings_saver(const char*);
	virtual operator std::ofstream*() noexcept;
	virtual ~settings_saver();
};

template<typename B>
class settings_base;

/**
 * Automazione per le impostazioni: carica e salva una variabile...
 * DA NON USARE PER DEL TESTO CON PIU' DI UNA RIGA.
 * Da usare insieme alla sua macro SETTINGS_ENTRY .
 * RENDIAMO GRAZIA a c++11 che con le sue nuove funzionalità
 * ha reso questo semplice da implementare
 *
 * @see SETTINGS_ENTRY
 */
template<typename T>
class settings_entry {
	const char* name;
	settings_io** io;

	template <typename B>
	friend class utilities::settings_base;

	settings_entry(const settings_entry<T> &) = default; // no copy
public:
	T value;
	settings_entry<T>& operator=(const settings_entry<T>&) = default;

	settings_entry(const char*_name, settings_io* &_io) :
			name(_name), io(&_io) {
		*this << **io;
	}

	inline operator T() const noexcept {
		return value;
	}

	inline T operator=(const T& val) {
		return value = val;
	}

	void operator<<(std::unordered_map<std::string, std::stringstream>*map) {
		if(map)
			if(map->count(name))
				map->at(name) >> value;
	}

	void operator>>(std::ofstream*out){
		//fixme: che orrore!
		(*out) << std::string(name) << '=' << value << std::endl;
	}

	inline ~settings_entry() {
		*this >> **io;
	}


};

template<>
class settings_entry<std::wstring> {
	const char* name;
	settings_io** io;
	settings_entry(const settings_entry<std::wstring> &) = default; // no copy
public:
	std::wstring value;

	settings_entry<std::wstring>& operator=(const settings_entry<std::wstring>&) = default;

	inline operator std::wstring() const noexcept {
		return value;
	}

	inline std::wstring operator=(const std::wstring& val) {
		return value = val;
	}

	settings_entry(const char*_name, settings_io* &_io);

	void operator<<(std::unordered_map<std::string, std::stringstream>*map);

	void operator>>(std::ofstream*out);

	inline ~settings_entry() {
		*this >> **io;
	}
};


template<>
class settings_entry<std::string> {
	const char* name;
	settings_io** io;
	settings_entry(const settings_entry<std::string> &) = default; // private copy
public:
	std::string value;
	settings_entry<std::string>& operator=(const settings_entry<std::string>&) = default;

	inline operator std::string() const noexcept {
		return value;
	}

	inline std::string operator=(const std::string& val) {
		return value = val;
	}

	settings_entry(const char*_name, settings_io* &_io);

	void operator<<(std::unordered_map<std::string, std::stringstream>*map);

	void operator>>(std::ofstream*out);

	inline ~settings_entry() {
		*this >> **io;
	}
};

/**
 * Segnala (e implementa) l'inizio della specifica di una classe di impostazioni.
 * Va usata sempre prima di ogni SETTINGS_ENTRY e SETTIGNS_END
 *
 * @see SETTINGS_ENTRY @see SETTIGNS_END
 */
#define SETTINGS_BEGIN(name, file) class name : public utilities::settings_base<name> { \
			inline name() : utilities::settings_base<name>(file) {\
				delete io;\
			}\
		public: \
		friend class utilities::singleton<name>;\
		friend class utilities::settings_base<name>


/**
 * Dichiara e implementa una impostazione di tipo T e nome name nella classe di impostazioni corrente.
 * Sarei potuto andare oltre e esporre un riferimento diretto alla variabile con il tipo corretto, ma per
 * ora mi accontento e lascio questo come TODO.
 */
#define SETTINGS_ENTRY(T, name) utilities::settings_entry<T> name {#name, io};


/**
 * Segnala la fine di una definiizone di una classe di impostazioni.
 */
#define SETTINGS_END( name ) ~name () { \
			io = new utilities::settings_saver(filename.c_str()); \
		} \
	}

/**
 * Implementazione dell'interfaccia settings base. Si è deciso che fosse un singleton.
 */
template <typename B>
class settings_base : public singleton<B> {
protected:
	std::string filename;
	settings_io*io;
public:
	settings_base(const char*f) : filename(f) {
		io = new settings_loader(f);
	}

	static void refresh() {
		B::inst() = B();
	}

	virtual ~settings_base(){
		delete io;
	}

};

}

#endif /* SOURCE_UTILITIES_INCLUDE_SETTINGS_HPP_ */
