
#ifndef SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_
#define SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_

#include <openssl/md5.h>
#include <string>
#include <algorithm>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <windows.h>
#endif

namespace utilities{
	int createDirectoryRecursively(const wchar_t* path);
	bool pathExists(const wchar_t*path);
	bool isPathDir(const wchar_t*path);

	template<typename C>
	std::basic_string<C> dirName(std::basic_string<C> s){
		s.erase(std::find(s.rbegin(), s.rend(), static_cast<C>('\\')).base(), s.end());
		return s;
	};

	template<typename C>
	std::basic_string<C> dirName(C* path){
		std::basic_string<C> s(path);
		s.erase(std::find(s.rbegin(), s.rend(), static_cast<C>('\\')).base()+1, s.end());
		return s;
	};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	bool fileMD5(const wchar_t*path, unsigned char buff[MD5_DIGEST_LENGTH]);
	bool fdMD5(HANDLE hwnd, unsigned char buff[MD5_DIGEST_LENGTH]);
#else
	bool fileMD5(const char*path, unsigned char buff[MD5_DIGEST_LENGTH]);
	bool fdMD5(int fd, unsigned char buff[MD5_DIGEST_LENGTH]);
#endif
}
#endif /* SOURCE_UTILITIES_INCLUDE_FSUTIL_HPP_ */
