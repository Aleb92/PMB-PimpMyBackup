
#ifndef SOURCE_CLIENT_INCLUDE_LOG_HPP_
#define SOURCE_CLIENT_INCLUDE_LOG_HPP_

#include <directorylistener.hpp>
#include <actionmerger.hpp>
#include <utilities/include/singleton.hpp>

#include <string>
#include <fstream>

namespace client {

struct log_entry_header  {
	char type;
	server::opcode op_code;
	FILETIME timestamp;
	size_t length;
};

/*
 * Il log...
 */
class log : public utilities::singleton<log> {
	friend class utilities::singleton<log>;
	log();
	FILE* log_file;
	~log();

	void issue(const file_action&, const std::wstring&);

public:

	void issue(const change_entity&);

	void finalize(const file_action&, const std::wstring&);
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_LOG_HPP_ */
