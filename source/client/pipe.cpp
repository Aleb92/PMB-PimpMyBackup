#include <settings.hpp>
#include <pipe.hpp>

#include <Windows.h>

using namespace client;
using namespace utilities;

pipe::pipe() {
	char buffer[1024];
	DWORD dwRead;

	hPipe = CreateNamedPipeA(settings::inst().pipe_name.value.c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 128, 1024,
			NMPWAIT_WAIT_FOREVER, nullptr);

	if (hPipe == INVALID_HANDLE_VALUE)
		throw base_exception();
}

pipe::~pipe() {
	CloseHandle(hPipe);
}

void pipe::driver() {
	char buffer[1024];
	DWORD dwRead;

	while (ConnectNamedPipe(hPipe, nullptr)) // wait for someone to connect to the pipe
	{

		on_return<> disconnect([this]() {
			DisconnectNamedPipe(this->hPipe);
		});

		while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, nullptr)) {
			//TODO read and stop
		}

		DWORD err = GetLastError();
		if(err != ERROR_BROKEN_PIPE)
			throw base_exception(err);

	}
	throw base_exception();
}

auth_exception::auth_exception() : base_exception("Wrong username/password") {
	pipe::inst().write<uint8_t>(WRONG_CREDENTIALS);
}

