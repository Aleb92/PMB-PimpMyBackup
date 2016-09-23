#include <debug.hpp>

using namespace utilities::debug;


int d_func::prefix = 0;

void utilities::debug::db_trace(void*, const char*query) {
	LOGD(query);
}
