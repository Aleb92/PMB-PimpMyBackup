
#ifndef SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_
#define SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_

#include <string>
#include <openssl/md5.h>


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <windows.h>
#endif

namespace utilities{
	int createDirectoryRecursively(const wchar_t* path);
	bool pathExists(const wchar_t*path);
	bool isPathDir(const wchar_t*path);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	bool fileMD5(const wchar_t*path, unsigned char buff[MD5_DIGEST_LENGTH]);
	bool fdMD5(HANDLE hwnd, unsigned char buff[MD5_DIGEST_LENGTH]);
#else
	bool fileMD5(const char*path, unsigned char buff[MD5_DIGEST_LENGTH]);
	bool fdMD5(int fd, unsigned char buff[MD5_DIGEST_LENGTH]);
#endif
}
#endif /* SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_ */
