#ifndef SOURCE_SERVER_INCLUDE_SETTINGS_HPP_
#define SOURCE_SERVER_INCLUDE_SETTINGS_HPP_


#include <utilities/include/socket.hpp>
#include <utilities/include/settings.hpp>

#include <string>

#define SETTINGS_FILE_NAME "settings.ini"

namespace server {
	SETTINGS_BEGIN(settings, SETTINGS_FILE_NAME);

	/**
	 * Host ip
	 */
	SETTINGS_ENTRY(std::string, server_host);

	/**
	 * Host listen port
	 */
	SETTINGS_ENTRY(in_port_t, server_port);

	/**
	 * Server queue size.
	 */
	SETTINGS_ENTRY(int, queue_size);

	/**
	 * Database name (in sqlite3 = filename)
	 */
	SETTINGS_ENTRY(std::string, db_name);

	/**
	 * Folder used to save user data
	 */
	SETTINGS_ENTRY(std::string, save_folder);

	SETTINGS_END(settings);
}

#endif /* SOURCE_SERVER_INCLUDE_SETTINGS_HPP_ */
