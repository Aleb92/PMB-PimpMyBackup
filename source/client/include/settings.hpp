
#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#define SETTINGS_FILE_NAME "settings.ini"

#include <string>
#include <locale>
#include <codecvt>
#include <utilities/include/singleton.hpp>

namespace client {


class settings : public utilities::singleton<settings> {
	friend class utilities::singleton<settings>;

	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	settings();
	virtual ~settings();
public:
	std::wstring log_filename;
	std::string tree_filename;
	std::wstring watched_dir;
	std::wstring server_host;
	uint16_t server_port;


	//TODO aggiungere altro

};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_ */
