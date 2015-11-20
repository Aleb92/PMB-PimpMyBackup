
#ifndef SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_
#define SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_

#include <string>

namespace utilities{
	int createDirectoryRecursively(const wchar_t* path);
	bool pathExists(const wchar_t*path);
	bool isPathDir(const wchar_t*path);
}

#endif /* SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_ */
