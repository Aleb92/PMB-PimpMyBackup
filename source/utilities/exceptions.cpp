#include <utilities/include/exceptions.hpp>

#include <exception>
#include <string>

using namespace utilities;
using namespace std;

base_exception::base_exception(const string& str) : msg(str) {}
base_exception::base_exception(string&& str) : msg(str) {}

virtual const char* base_exception::what() const {
	return msg.c_str();
}

#define IMPLEMENT_EXCEPTION(name, message) \
	name::name(const string& str) : base_exception(str){}\
	name::name(string&& str) : base_exception(str){}


