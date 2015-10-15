
#ifndef SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_
#define SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_

#include <Shlobj.h>

namespace utilities{

	int createDirectoryRecursively(const char* path)
	{
		return SHCreateDirectoryEx( 0, path, 0);
	}
}

#endif /* SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_ */
