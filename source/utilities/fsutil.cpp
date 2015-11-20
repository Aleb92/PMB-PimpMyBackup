#include <utilities/include/fsutil.hpp>
#include <openssl/md5.h>
#include <functional>

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>

int utilities::createDirectoryRecursively(const wchar_t* path) {
	// FIXME: orribile!
	int result = _wsystem((std::wstring(L"mkdir ") + path).c_str());
	return result;
}

bool utilities::pathExists(const wchar_t*path) {
	return ::GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
}

bool utilities::isPathDir(const wchar_t*path) {
	DWORD attr = GetFileAttributesW(path);
	return (attr != INVALID_FILE_ATTRIBUTES)
			&& (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool utilities::fileMD5(const wchar_t*path, unsigned char buff[MD5_DIGEST_LENGTH]) {
	HANDLE file = CreateFileW(path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	bool ret = fdMD5(file, buff);
	CloseHandle(file);
	return ret;
}

bool utilities::fdMD5(HANDLE hwnd, unsigned char buff[MD5_DIGEST_LENGTH]) {
	LARGE_INTEGER size;
	if (!GetFileSizeEx(hwnd, &size))
		return false;

	HANDLE map = CreateFileMapping(hwnd,
					NULL,
					PAGE_READONLY, size.HighPart, size.LowPart,
					NULL);

	if(map == NULL)
		return false;

	const unsigned char*fileBUFF = static_cast<unsigned char*>( MapViewOfFile(
			 map,
			 FILE_MAP_READ,
			 0, 0, 0) );
	if(fileBUFF == NULL) {
		CloseHandle(map);
		return false;
	}

	MD5(fileBUFF, (size_t)(size.QuadPart), buff);

	UnmapViewOfFile(fileBUFF);
	CloseHandle(map);

	return true;
}

#else

//TODO: unix utilities

#endif
