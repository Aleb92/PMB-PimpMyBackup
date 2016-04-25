#include <exceptions.hpp>
#include <iostream>

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

base_exception::base_exception(const string& str, int l, const char* f, const char* ff) noexcept : msg(str), line(l), fn(f), file(ff) {}
base_exception::base_exception(string&& str, int l, const char* f, const char* ff) noexcept : msg(str), line(l), fn(f), file(ff) {}

const char* base_exception::what() const noexcept {
	cout << "In file: " << file << " function: " << fn << " line: " << line  << endl;
	return msg.c_str();
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
base_exception::base_exception(DWORD code, int l, const char* f, const char* ff) noexcept : line(l), fn(f), file(ff) {

	char *s = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, code,
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   (LPTSTR)&s, 0, NULL);
	msg = string(s);
	LocalFree(s);
}

base_exception::base_exception(int l, const char* f, const char* ff) noexcept : line(l), fn(f), file(ff) {

	char *s = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		               NULL, GetLastError(),
		               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					   (LPTSTR)&s, 0, NULL);
		msg = string(s);
		LocalFree(s);
}

#else
base_exception::base_exception(int code, int l, const char* f, const char* ff) noexcept : line(l), fn(f), file(ff) {
	msg = string(strerror(code));
}

base_exception::base_exception(int l, const char* f, const char* ff) noexcept : line(l), fn(f), file(ff) {
	msg = string(strerror(errno));
}
#endif


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
socket_exception::socket_exception(int l, const char* f, const char* ff) noexcept : base_exception(WSAGetLastError(), l, f, ff) {

#else
socket_exception::socket_exception(int l, const char* f, const char* ff) noexcept : base_exception(errno, l, f, ff) {
#endif
}

db_exception::db_exception(int err, int l, const char* f, const char* ff) noexcept : base_exception(sqlite3_errstr(err), l, f, ff){
}

db_exception::db_exception(sqlite3* db, int l, const char* f, const char* ff) noexcept : base_exception(sqlite3_errmsg(db), l, f, ff){
}

db_exception::db_exception(char* err, int l, const char* f, const char* ff) noexcept : base_exception(err, l, f, ff){
	sqlite3_free(err);
}


