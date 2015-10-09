/*
 * atend.hpp
 *
 *  Created on: 06/ott/2015
 *      Author: Marco
 */

#ifndef ATEND_HPP_
#define ATEND_HPP_

#include "traits.hpp"
#include <iostream>
#include <type_traits>

namespace utilities {

	/**
	 *  Questa classe è la controparte di at_begin. Serve a fare pulizia.
	 */
	template <typename T, T (*func)()>
	class at_end {
	private:
		at_end() {}
		~at_end() {
			(void)func();
		}
		static const at_end<T,func> caller;
	};

	template <typename T, T (*func)()> const at_end<T, func> at_end<T, func>::caller;

#define atEnd(func) template class utilities::at_end<decltype(func()), func>

}

#endif /* ATEND_HPP_ */
