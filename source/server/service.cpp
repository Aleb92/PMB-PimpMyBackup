#include <settings.hpp>
#include <database.hpp>

#include <utilities/include/socket.hpp>
#include <utilities/include/threadpool.hpp>
#include <utilities/include/atend.hpp>

#include <thread>
#include <vector>
#include <memory>
#include <Windows.h>

using namespace std;
using namespace utilities;
using namespace server;

thread t;

SERVICE_STATUS g_ServiceStatus = { SERVICE_WIN32_OWN_PROCESS,
SERVICE_START_PENDING };

SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv);
VOID WINAPI ServiceCtrlHandler(DWORD) {
}

void work() { }

#define SERVICE_NAME  L"PMB"

void start_service() {
	SERVICE_TABLE_ENTRYW ServiceTable[] = { { SERVICE_NAME,
			(LPSERVICE_MAIN_FUNCTIONW) ServiceMain }, { NULL, NULL } };

	if (!StartServiceCtrlDispatcherW(ServiceTable))
		throw base_exception();
}

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv) {
	try {
	DWORD Status = E_FAIL;

	// Register our service control handler with the SCM
	g_StatusHandle = RegisterServiceCtrlHandlerW(SERVICE_NAME,
			ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
		return;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		OutputDebugString("PMB: SetServiceStatus returned error");


	// Create a service stop event to wait on later
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL) {
		// Error creating event
		// Tell service controller we are stopped and exit
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
			throw base_exception();
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		throw base_exception();


		// Start a thread that will perform the main task of the service
		th = std::move(thread(work));

		// Wait until our worker thread exits signaling that the service needs to stop
		WaitForSingleObject(hThread, INFINITE);

		/*
		 * Perform any cleanup tasks
		 */

		CloseHandle(g_ServiceStopEvent);

		// Tell the service controller we are stopped
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 3;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
			OutputDebugString(
					_T(
							"My Sample Service: ServiceMain: SetServiceStatus returned error"));
		}
	}
	catch (base_exception& be) {
		OutputDebugStringA(be.what());
	}
}
