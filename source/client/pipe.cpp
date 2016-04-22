#include <settings.hpp>
#include <pipe.hpp>

#include <iostream>
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
		throw base_exception(__LINE__, __func__, __FILE__);
}

pipe::~pipe() {
	CloseHandle(hPipe);
}

void pipe::driver() {
	try {
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
			if (err != ERROR_BROKEN_PIPE)
				throw base_exception(err,__LINE__, __func__, __FILE__);

		}
		throw base_exception(__LINE__, __func__, __FILE__);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

auth_exception::auth_exception(int l, const char* f, const char* ff) :
		base_exception("Wrong username/password", l,f,ff) {
	pipe::inst().write<uint8_t>(WRONG_CREDENTIALS);
}

