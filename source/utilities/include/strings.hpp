#ifndef SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_
#define SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_

#include <cwchar>
#include <stddef.h>
#include <vector>
#include <string>

namespace utilities{

const wchar_t* wcsrchr (const wchar_t*, wchar_t, size_t);

std::vector<std::string> split(const std::string &, char);
std::vector<std::string> &split(const std::string &s, char delim,
		std::vector<std::string> &elems);

std::vector<std::wstring> split(const std::wstring &, wchar_t);
std::vector<std::wstring> &split(const std::wstring &s, wchar_t delim,
		std::vector<std::wstring> &elems);

std::pair<std::wstring, std::wstring> splitOnce(const std::wstring &s, wchar_t delim);
}
#endif /* SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_ */
