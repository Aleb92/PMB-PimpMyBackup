/*
 * service.hpp
 *
 *  Created on: 06 apr 2016
 *      Author: mrcmn
 */

#ifndef SOURCE_SERVER_INCLUDE_SERVICE_HPP_
#define SOURCE_SERVER_INCLUDE_SERVICE_HPP_

#include <utilities/include/atend.hpp>
#include <Windows.h>

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
void start_service();

#endif /* SOURCE_SERVER_INCLUDE_SERVICE_HPP_ */
