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
		const std::string msg;
	public:
		base_exception(const std::string&);
		base_exception(std::string&&);

		virtual const char* what() const;
	};

	//TODO definire le eccezioni

}


#endif /* SOURCE_UTILITIES_INCLUDE_EXCEPTIONS_HPP_ */
