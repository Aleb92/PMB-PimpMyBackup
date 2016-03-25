#include <settings.hpp>
#include <strings.hpp>

#include <cwchar>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include <fstream>

using namespace std;
using namespace utilities;

wstring_convert<codecvt_utf8<wchar_t>> settings_entry<wstring>::converter;

// Prima la base: settings I/O
settings_io::~settings_io() { }
settings_io::operator ofstream*() noexcept { return nullptr; }
settings_io::operator unordered_map<string, stringstream>*()
		noexcept{ return nullptr; }

// Poi in ordine loader
settings_loader::settings_loader(const char*filename) {
	string line;
	ifstream settingFile (filename);

	if (settingFile.is_open())
	  {
	    while ( getline(settingFile,line) )
	    {
	    	pair<string, stringstream> result = utilities::splitOnce(line, '=');
	    	map[result.first].swap(result.second);
	    }
	    settingFile.close();
	  }
}

settings_loader::~settings_loader() {}

settings_loader::operator unordered_map<string, stringstream>*()
		noexcept{ return &map; }

// E il saver
settings_saver::settings_saver(const char*file) : out(file, ios_base::out | ios_base::trunc) {}

settings_saver::operator std::ofstream*() noexcept {
	return &out;
}

settings_saver::~settings_saver() {
	out.close();
}


// ora le entry vere e proprie
settings_entry<wstring>::settings_entry(const char*_name, settings_io* &_io) :
		name(_name), io(_io) {
	*this << *io;
}

void settings_entry<wstring>::operator<<(std::unordered_map<std::string, std::stringstream>*map) {
	if(map)
		if(map->count(name)) {
			string tmp;
			getline(map->at(name), tmp);
			value = converter.from_bytes(tmp);
		}
}

void settings_entry<wstring>::operator>>(std::ofstream*out){
	string tmp = converter.to_bytes(value);
	(*out) << name << '=' << tmp << std::endl;
}

settings_entry<string>::settings_entry(const char*_name, settings_io* &_io) :
		name(_name), io(_io) {
	*this << *io;
}

void settings_entry<string>::operator<<(std::unordered_map<std::string, std::stringstream>*map) {
	if(map)
		if(map->count(name)) {
			getline(map->at(name), value);
		}
}

void settings_entry<string>::operator>>(std::ofstream*out){
	(*out) << name << '=' << value << std::endl;
}

