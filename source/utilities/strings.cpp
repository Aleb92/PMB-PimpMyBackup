#include <utilities/include/strings.hpp>

#include <sstream>
#include <vector>
#include <string>

using namespace std;

namespace utilities {

const wchar_t* wcsrchr(const wchar_t*str, wchar_t lk, size_t last) {
	for (const wchar_t * ret = str + last; ret != str; ret--) {
		if (*ret == lk)
			return ret;
	}
	return nullptr;
}

std::pair<std::wstring, std::wstring> splitOnce(const std::wstring &s, wchar_t delim){
	std::wstringstream ss(s);
	std::wstring item;
	std::pair<std::wstring, std::wstring> p;
	std::getline(ss, item, delim);
	p.first = item;
	p.second = wstring(istreambuf_iterator<wchar_t>(ss), {});
	return p;
}

std::pair<std::string, std::string> splitOnce(const std::string &s, char delim){
	std::stringstream ss(s);
	std::string item;
	std::pair<std::string, std::string> p;
	std::getline(ss, item, delim);
	p.first = item;
	p.second = string(istreambuf_iterator<char>(ss), {});
	return p;
}


std::vector<std::wstring> split(const std::wstring &s, wchar_t delim) {
	std::vector<std::wstring> elems;
	split(s, delim, elems);
	return elems;
}

std::vector<std::wstring> &split(const std::wstring &s, wchar_t delim,
		std::vector<std::wstring> &elems) {
	std::wstringstream ss(s);
	std::wstring item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::vector<std::string> &split(const std::string &s, char delim,
		std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
}
