/*
 * resources.h
 *
 *  Created on: Sep 29, 2015
 *      Author: s180678
 */

#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <unistd.h>

namespace utilities {


	class unique_ures {
	protected:
		int fd;
	public:
		unique_ures(const unique_ures&) = delete;
		unique_ures(int _fd) : fd(_fd) { 	}
	};



}


#endif /* RESOURCES_H_ */
