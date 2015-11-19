/*
 * settings.cpp
 *
 *  Created on: 13 nov 2015
 *      Author: Marco
 */

#include <settings.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace client {

settings::settings() {
	wstring line;
	wifstream myfile (SETTINGS_FILE_NAME);
	if (myfile.is_open())
	  {
	    while ( getline(myfile,line) )
	    {


	    }
	    myfile.close();
	  }
}

settings::~settings() {
	// TODO Auto-generated destructor stub
}

} /* namespace client */
