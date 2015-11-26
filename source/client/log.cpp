/*
 * log.cpp
 *
 *  Created on: 13 nov 2015
 *      Author: Marco
 */

#include <settings.hpp>
#include <log.hpp>
#include <windows.h>
#include <io.h>

using namespace std;
using namespace client;

log::log() {
	wstring oldfile_name = settings::inst().log_filename + L".old";
	if(MoveFileW(settings::inst().log_filename.c_str(), oldfile_name.c_str())){
		//TODO: Load old log
	}

	HANDLE file_handle = CreateFileW(
			settings::inst().log_filename.c_str(),
			GENERIC_WRITE,
		    0, NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH, NULL);

	if (file_handle != INVALID_HANDLE_VALUE) {
	    int file_descriptor = _open_osfhandle((intptr_t)file_handle, 0);

	    if (file_descriptor != -1) {
	        FILE* file = _fdopen(file_descriptor, "w");

	        if (file != NULL) {
//	            log_file = ofstream(file);
	            log_file.rdbuf()->pubsetbuf(nullptr, 0);
	        }
	    }
	}
}

log::~log() {
	log_file.close();
}
