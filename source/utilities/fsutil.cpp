#include <utilities/include/fsutil.hpp>

using namespace std;
using namespace utilities;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>

int utilities::createDirectoryRecursively(const wchar_t* path)
{
	//int result = SHCreateDirectory( 0, path );
	// FIXME: orribile!
	int result = _wsystem((std::wstring(L"mkdir ") + path).c_str());
	return result;
}

bool utilities::pathExists(const wchar_t*path) {
	return ::GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

bool utilities::isPathDir(const wchar_t*path) {
	DWORD attr = GetFileAttributesW(path);
	return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

#else

//TODO: unix utilities

#endif
