#include <settings.hpp>
#include <client.hpp>

#include <thread>
#include <mutex>
#include <Windows.h>

using namespace std;
using namespace utilities;
using namespace client;

client::client c;
mutex l;

SERVICE_STATUS g_ServiceStatus = { SERVICE_WIN32_OWN_PROCESS,
SERVICE_START_PENDING };

SERVICE_STATUS_HANDLE g_StatusHandle = NULL;

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv);
VOID WINAPI ServiceCtrlHandler(DWORD);

#define SERVICE_NAME  L"PMB"

void start_service() {
	SERVICE_TABLE_ENTRYW ServiceTableEntry[] = { { const_cast<wchar_t*>(SERVICE_NAME),
			(LPSERVICE_MAIN_FUNCTIONW) ServiceMain }, { 0 } };

	if (!StartServiceCtrlDispatcherW(ServiceTableEntry))
		throw base_exception();
}

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv) {
	try {
	    lock_guard<mutex> guard(l);
		DWORD Status = E_FAIL;

		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
			throw base_exception();

		// Register our service control handler with the SCM
		g_StatusHandle = RegisterServiceCtrlHandlerW(SERVICE_NAME,
				ServiceCtrlHandler);

		if (g_StatusHandle == NULL)
			throw base_exception();

		// Tell the service controller we are started
		g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PARAMCHANGE;
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;

		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
			throw base_exception();

		// Start a thread that will perform the main task of the service
        c.start();

        l.lock();

        c.stop();

		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;

		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
		    throw base_exception();

	} catch (base_exception& be) {
	    OutputDebugStringA(be.what());
	}
}

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
    switch (CtrlCode) {

    case SERVICE_ACCEPT_PARAMCHANGE:
        settings::refresh();
        c.stop();
        c = std::move(client::client());
        break;

    case SERVICE_CONTROL_SHUTDOWN :
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
           break;

        g_ServiceStatus.dwCheckPoint = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;

        if (!SetServiceStatus (g_StatusHandle, &g_ServiceStatus))
            OutputDebugString("Errore in chiusura");

        l.unlock();

        break;

    case SERVICE_CONTROL_INTERROGATE:

        g_ServiceStatus.dwCheckPoint++;
        if (!SetServiceStatus (g_StatusHandle, &g_ServiceStatus))
            OutputDebugString("Errore nel settare lo stato");

        break;
    default:
        break;
    }
}
