/*
 * exceptions.hpp
 *
 *  Created on: 01 dic 2015
 *      Author: alessio
 */

#ifndef SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_
#define SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_

#include <string>
#include <exception>

namespace utilities {
	class base_exception : public std::exception {
		const std::string msg;//FIXME: const char* non ci piace?
	public:
		base_exception(const std::string&);
		base_exception(std::string&&);

		virtual const char* what() const noexcept;
	};


#define IMPLEMENT_EXCEPTION(name) \
	class name : public base_exception { \
	public: \
		inline name(const string& str) : base_exception(#name + ":" + str){} \
		inline name::name(string&& str) : base_exception(#name + ":" + str) {} \
	}

	//TODO Bonato: definire le eccezioni

}


#endif /* SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_ */
