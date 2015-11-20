
#include <settings.hpp>
#include <utilities/include/strings.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cwchar>

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

	log_filename = settings["log_filename"];
	tree_filename = settings["tree_filename"];
	watched_dir= settings["watched_dir"];
	server_host= settings["server_host"];

	wstring port = settings["server_port"];
	server_port = static_cast<uint16_t>(_wtoi(port.c_str()));
}

settings::~settings() {

	wofstream settingFile(SETTINGS_FILE_NAME);
	if(settingFile.is_open()){
		settingFile << "log_filename=" << log_filename << endl;
		settingFile << "tree_filename=" << tree_filename << endl;
		settingFile << "watched_dir=" << watched_dir << endl;
		settingFile << "server_host=" << server_host << endl;
		settingFile << "server_port=" << server_port << endl;
	}

	settingFile.close();
}

} /* namespace client */
