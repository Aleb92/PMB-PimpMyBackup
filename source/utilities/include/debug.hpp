/*
 * debug.hpp
 *
 *  Created on: 25 apr 2016
 *      Author: mrcmn
 */

#ifndef SOURCE_UTILITIES_INCLUDE_DEBUG_HPP_
#define SOURCE_UTILITIES_INCLUDE_DEBUG_HPP_
#include <utilities/include/strings.hpp>

#include <thread>
#include <iostream>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# include <Windows.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		std::ostream& operator<< (std::ostream& out, FILETIME);
#endif

namespace utilities {

	namespace debug {

		void db_trace(void*, const char*);


		struct d_func {
			static int prefix;
			const char* const c;
			inline d_func(const char* __f) : c(__f){
				std::cout << std::this_thread::get_id() << std::string(prefix, ' ') << "Entro in: " << __f << std::endl;
				prefix += 2;
			}

			~d_func() {
				prefix -= 2;
				std::cout << std::this_thread::get_id() << std::string(prefix, ' ') << "Esco da: " << c << std::endl;
			}
		};
	}

#define LOGD(a) std::cout << std::this_thread::get_id() << std::string(utilities::debug::d_func::prefix, ' ') << a << std::endl
#define LOGF utilities::debug::d_func d_func(__func__)

}
#endif /* SOURCE_UTILITIES_INCLUDE_DEBUG_HPP_ */
