#include <settings.hpp>
#include <client.hpp>

#include <thread>
#include <mutex>
#include <Windows.h>

using namespace std;
using namespace utilities;
using namespace client;

unique_ptr<client::client> c;
mutex l;

SERVICE_STATUS g_ServiceStatus = { SERVICE_WIN32_OWN_PROCESS,
SERVICE_START_PENDING };

SERVICE_STATUS_HANDLE g_StatusHandle = NULL;

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv);
VOID WINAPI ServiceCtrlHandler(DWORD);

#define SERVICE_NAME  L"PMB"

void start_service() {
	LOGF;
	c.reset(new client::client());
	SERVICE_TABLE_ENTRYW ServiceTableEntry[] = { { const_cast<wchar_t*>(SERVICE_NAME),
			(LPSERVICE_MAIN_FUNCTIONW) ServiceMain }, { 0 } };

	if (!StartServiceCtrlDispatcherW(ServiceTableEntry))
		throw base_exception(__LINE__, __func__, __FILE__);
}

VOID WINAPI ServiceMain(DWORD argc, wchar_t **argv) {
	LOGF;
	try {
	    lock_guard<mutex> guard(l);
		DWORD Status = E_FAIL;

		// Register our service control handler with the SCM
		g_StatusHandle = RegisterServiceCtrlHandlerW(SERVICE_NAME,
				ServiceCtrlHandler);
		if (g_StatusHandle == NULL)
			throw base_exception(__LINE__, __func__, __FILE__);

		LOGD("Service registered");

		// Tell the service controller we are started
		g_ServiceStatus.dwControlsAccepted =
				SERVICE_ACCEPT_SHUTDOWN |
				SERVICE_ACCEPT_PARAMCHANGE |
				SERVICE_ACCEPT_STOP;
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;

		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
			throw base_exception(__LINE__, __func__, __FILE__);

		LOGD("Manager updated");

		LOGD("Starting client...");

		// Start a thread that will perform the main task of the service
        c->start();
		LOGD("Started");

        l.lock();

		LOGD("Stop request received");

        c->stop();

        LOGD("Sending termination state to the manager");

        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwCheckPoint = -1;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwControlsAccepted = 0;


		if (!SetServiceStatus(g_StatusHandle, &g_ServiceStatus))
		    throw base_exception(__LINE__, __func__, __FILE__);

		LOGD("Ending operations...");

	} catch (base_exception& be) {
	    OutputDebugStringA(be.what());
	}
}

VOID WINAPI ServiceCtrlHandler (DWORD CtrlCode)
{
    switch (CtrlCode) {

    case SERVICE_ACCEPT_PARAMCHANGE:
        settings::refresh();
        c->stop();
    	c.reset(new client::client());
        break;

    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
           break;

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCheckPoint = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;

        if (!SetServiceStatus (g_StatusHandle, &g_ServiceStatus))
            cout << "Errore in chiusura";

        l.unlock();

        break;

    case SERVICE_CONTROL_INTERROGATE:
    	LOGD("Piazzo il checkpoint: " + action_merger::inst().pending_count);
        g_ServiceStatus.dwCheckPoint = action_merger::inst().pending_count;
        if (!SetServiceStatus (g_StatusHandle, &g_ServiceStatus))
            OutputDebugString("Errore nel settare lo stato");

        break;
    default:
        break;
    }
}
