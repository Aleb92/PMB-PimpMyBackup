#include <settings.hpp>
#include <utilities/include/strings.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <cwchar>
#include <locale>
#include <codecvt>

using namespace std;

namespace client {

std::wstring_convert<std::codecvt_utf8<wchar_t>> settings::converter;


} /* namespace client */
