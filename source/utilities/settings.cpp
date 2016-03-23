#include <settings.hpp>

#include <cwchar>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>

using namespace std;
using namespace utilities;

static wstring_convert<codecvt_utf8<wchar_t>> settings_entry<wstring>::converter;

settings_entry<wstring>::settings_entry(const char*_name, settings_io* &_io) :
		name(_name), io(_io) {
	*this << *io;
}

void settings_entry<wstring>::operator<<(const std::unordered_map<std::string, std::stringstream>*map) {
	if(map)
		if(map->count(name)) {
			string tmp;
			getline(((*map)[name]), tmp);
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

void settings_entry<string>::operator<<(const std::unordered_map<std::string, std::stringstream>*map) {
	if(map)
		if(map->count(name)) {
			getline(((*map)[name]), value);
		}
}

void settings_entry<string>::operator>>(std::ofstream*out){
	(*out) << name << '=' << value << std::endl;
}
