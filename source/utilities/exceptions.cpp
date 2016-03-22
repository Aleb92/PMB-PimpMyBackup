#include <utilities/include/exceptions.hpp>

#include <exception>
#include <string>

using namespace utilities;
using namespace std;

base_exception::base_exception(const string& str) : msg(str) {}
base_exception::base_exception(string&& str) : msg(str) {}

const char* base_exception::what() const noexcept {
	return msg.c_str();
}



