#include <debug.hpp>

using namespace utilities::debug;


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	std::ostream& operator<< (std::ostream& out, FILETIME time) {
		ULARGE_INTEGER large;
		large.HighPart = time.dwHighDateTime;
		large.LowPart = time.dwLowDateTime;
		return out << large.QuadPart;
	}
#endif

int d_func::prefix = 0;

void utilities::debug::db_trace(void*, const char*query) {
	LOGD(query);
}
