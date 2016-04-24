#include <utilities/include/strings.hpp>

#include <sstream>
#include <vector>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <windows.h>
#endif

using namespace std;
using namespace utilities;

//TODO: fare per linux
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
std::string utf8_encode(const std::wstring &wstr)
{
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
    if( str.empty() ) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
#else

#include <codecvt>
#include <locale>

static wstring_convert<codecvt_utf8<wchar_t>> u8cvt;

string utf8_encode(const wstring& wstr) {
	return u8cvt.to_bytes(wstr);
}

wstring utf8_decode(const string& str) {
	return u8cvt.from_bytes(str);
}
#endif

const wchar_t* wcsrchr(const wchar_t*str, wchar_t lk, size_t last) {
	for (const wchar_t * ret = str + last; ret != str; ret--) {
		if (*ret == lk)
			return ret;
	}
	return nullptr;
}

pair<wstring, wstring> splitOnce(const wstring &s, wchar_t delim){
	wstringstream ss(s);
	wstring item;
	pair<wstring, wstring> p;
	getline(ss, item, delim);
	p.first = item;
	p.second = wstring(istreambuf_iterator<wchar_t>(ss), {});
	return p;
}

pair<string, stringstream> splitOnce(const string &s, char delim){
	pair<string, stringstream> p = make_pair(string(), stringstream(s));
	getline(p.second, p.first, delim);
	return p;
}


vector<wstring> split(const wstring &s, wchar_t delim) {
	vector<wstring> elems;
	split(s, delim, elems);
	return elems;
}

vector<wstring> &split(const wstring &s, wchar_t delim,
		vector<wstring> &elems) {
	wstringstream ss(s);
	wstring item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

vector<string> &split(const string &s, char delim,
		vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

