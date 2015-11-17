/*
 * singleton.hpp
 *
 *  Created on: 13 nov 2015
 *      Author: Marco
 */

#ifndef SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_
#define SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_

namespace utilities {

	template <typename T>
	class singleton{
	public:
		static inline T& inst() {
			static T inst;
			return inst;
		}
	};

}

#endif /* SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_ */
