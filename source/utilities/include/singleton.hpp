/*
 * singleton.hpp
 *
 *  Created on: 13 nov 2015
 *      Author: Marco
 */

#ifndef SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_
#define SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_

namespace utilities {

	/**
	 * Classe di helper che serve a definire un singleton
	 * tramite il paradigma CRTP.
	 * La politica di gestione di vita del singleton è
	 * statica con lazy initializzation.
	 */
	template <typename T>
	class singleton{
	public:
		static inline T& inst() {
			static T val;
			return val;
		}
	};

}

#endif /* SOURCE_UTILITIES_INCLUDE_SINGLETON_HPP_ */
