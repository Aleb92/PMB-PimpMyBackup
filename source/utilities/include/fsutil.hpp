
#ifndef SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_
#define SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_

#include <string>
#include <Shlobj.h>

namespace utilities{

	int createDirectoryRecursively(const wchar_t* path)
	{
		//int result = SHCreateDirectory( 0, path );
		int result = _wsystem((std::wstring(L"mkdir ") + path).c_str());
		return result;
	}
}

#endif /* SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_ */
