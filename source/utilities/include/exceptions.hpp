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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <Windows.h>
#endif
# include <cerrno>
namespace utilities {

/**
 *	Classe di base per tutte le nostre eccezioni,
 *	non istanziabile di per se
 */
class base_exception: public std::exception {
protected:
	std::string msg; //FIXME: const char* non ci piace?
public:
	int line;
	const char* fn, * file;

	base_exception(const std::string&, int line, const char* function, const char* file) noexcept;
	base_exception(const char*, int line, const char* function, const char* file) noexcept;
	base_exception(int line, const char* function, const char* file) noexcept;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	base_exception(DWORD code, int line, const char* function, const char* file) noexcept;
#else
	base_exception(int code, int line, const char* function, const char* file) noexcept;
#endif
	virtual const char* what() const noexcept;
};

#define IMPLEMENT_EXCEPTION(name) \
	class name : public base_exception { \
	public: \
		using base_exception::base_exception; \
	}

IMPLEMENT_EXCEPTION(fs_exception);
IMPLEMENT_EXCEPTION(memory_exception);


class io_exception : public base_exception {
public:
	using base_exception::base_exception;
};

class socket_exception: public base_exception {
public:
	socket_exception(int line, const char* function, const char* file) noexcept;
};

class db_exception: public base_exception {
public:
	using base_exception::base_exception;
	db_exception(int line, const char* function, const char* file) = delete;
	db_exception(int, int line, const char* function, const char* file) noexcept;
	db_exception(sqlite3*, int line, const char* function, const char* file) noexcept;
	db_exception(char *, int line, const char* function, const char* file) noexcept;
};

}

#endif /* SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_ */
