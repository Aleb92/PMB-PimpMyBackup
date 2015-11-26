
#include <settings.hpp>
#include <utilities/include/strings.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cwchar>
#include <locale>
#include <codecvt>

using namespace std;

namespace client {

settings::settings() {
	wstring line;
	wifstream settingFile (SETTINGS_FILE_NAME);
	map<wstring, wstring> settings;

	if (settingFile.is_open())
	  {
	    while ( getline(settingFile,line) )
	    {
	    	pair<wstring, wstring> result = utilities::splitOnce(line, L'=');
	    	settings[result.first]= result.second;
	    }
	    settingFile.close();
	  }

	log_filename = settings[L"log_filename"];

	wstring_convert<codecvt_utf8<wchar_t>> converter;
	tree_filename = converter.to_bytes(settings[L"tree_filename"]);

	watched_dir = settings[L"watched_dir"];
	server_host = settings[L"server_host"];

	wstring port = settings[L"server_port"];
	server_port = static_cast<uint16_t>(_wtoi(port.c_str()));
}

settings::~settings() {

	wofstream settingFile(SETTINGS_FILE_NAME);
	if(settingFile.is_open()){
		settingFile << L"log_filename=" << log_filename << endl;
		settingFile << L"tree_filename=" << tree_filename << endl;
		settingFile << L"watched_dir=" << watched_dir << endl;
		settingFile << L"server_host=" << server_host << endl;
		settingFile << L"server_port=" << server_port << endl;
	}

	settingFile.close();
}

} /* namespace client */
