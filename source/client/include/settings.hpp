/*
 * settings.hpp
 *
 *  Created on: 13 nov 2015
 *      Author: Marco
 */

#ifndef SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_
#define SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_

#include <string>
#include <utilities/include/singleton.hpp>

namespace client {


class settings : public utilities::singleton<settings> {
	friend class utilities::singleton<settings>;
	settings();
	virtual ~settings();
public:
	std::wstring log_filename;
	std::wstring watched_dir;
	std::wstring server_host;
	uint16_t server_port;

	//TODO aggiungere altro

};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_SETTINGS_HPP_ */
