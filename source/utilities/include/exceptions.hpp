/*
 * exceptions.hpp
 *
 *  Created on: 01 dic 2015
 *      Author: alessio
 */

#ifndef SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_
#define SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_

#include <string>
#include <exception>
#include <sqlite3.h>

namespace utilities {
class base_exception: public std::exception {
protected:
	std::string msg; //FIXME: const char* non ci piace?
public:
	base_exception(const std::string&);
	base_exception(std::string&&);
	base_exception(void);
	virtual const char* what() const noexcept;
};

#define IMPLEMENT_EXCEPTION(name) \
	class name : public base_exception { \
	public: \
		using base_exception::base_exception; \
	}

IMPLEMENT_EXCEPTION(fs_exception);
IMPLEMENT_EXCEPTION(auth_exception);
IMPLEMENT_EXCEPTION(memory_exception);

class socket_exception: public base_exception {
public:
	socket_exception(void);
};

class db_exception: public base_exception {
public:
	using base_exception::base_exception;
	db_exception(void) = delete;
	db_exception(int);
	db_exception(sqlite3*);
	db_exception(char *);
};

}

#endif /* SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_ */
