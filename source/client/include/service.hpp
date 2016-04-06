/*
 * service.hpp
 *
 *  Created on: 06 apr 2016
 *      Author: mrcmn
 */

#ifndef SOURCE_SERVER_INCLUDE_SERVICE_HPP_
#define SOURCE_SERVER_INCLUDE_SERVICE_HPP_

#include <Windows.h>

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv);
VOID WINAPI ServiceCtrlHandler(DWORD) {}
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam) { return 0; }
void start_service();

#endif /* SOURCE_SERVER_INCLUDE_SERVICE_HPP_ */
