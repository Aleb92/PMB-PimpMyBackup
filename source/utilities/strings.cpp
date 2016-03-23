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
	stringstream ss(s);
	string item;
	pair<string, stringstream> p;
	getline(ss, item, delim);
	p.first = item;
	p.second = ss;
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
}
