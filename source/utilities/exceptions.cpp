#include <exceptions.hpp>

#include <exception>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#else
#include <cstring>
#include <cerrno>
#endif

using namespace utilities;
using namespace std;

base_exception::base_exception(const string& str) : msg(str) {}
base_exception::base_exception(string&& str) : msg(str) {}

const char* base_exception::what() const noexcept {
	return msg.c_str();
}

base_exception::base_exception(){
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

	char *s = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, GetLastError(),
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   (LPTSTR)&s, 0, NULL);
	msg = string(s);
	LocalFree(s);

#else
	msg = string(strerror(errno));
#endif
}

socket_exception::socket_exception(){
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

	char *s = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, WSAGetLastError(),
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   (LPTSTR)&s, 0, NULL);
	msg = string(s);
	LocalFree(s);

#else
	msg = string(strerror(errno));
#endif
}

db_exception::db_exception(int err) : base_exception(sqlite3_errstr(err)){
}

db_exception::db_exception(sqlite3* db): base_exception(sqlite3_errmsg(db)){
}

db_exception::db_exception(char* err) : base_exception(err){
	sqlite3_free(err);
}



