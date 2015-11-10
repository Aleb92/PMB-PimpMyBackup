#include <strings.hpp>

const wchar_t* wcsrchr (const wchar_t*str, wchar_t lk, size_t last) {
	for(const wchar_t * ret = str + last; ret != str; ret--) {
		if(*ret == lk)
			return ret;
	}
	return nullptr;
}
