#include <settings.hpp>
#include <pipe.hpp>
#include <actionmerger.hpp>

#include <iostream>
#include <Windows.h>

using namespace client;
using namespace utilities;
using namespace std;

pipe::pipe() {
	LOGF;
	char buffer[1024];
	DWORD dwRead;

	//Questo dovrebbe permettere a tutti di accedere
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!pSD)
		throw base_exception(__LINE__, __func__, __FILE__);

	on_return<>([pSD]() {
		LocalFree(pSD);
	});

	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
		throw base_exception(__LINE__, __func__, __FILE__);

	if (!SetSecurityDescriptorDacl(pSD, TRUE, NULL, FALSE))
		throw base_exception(__LINE__, __func__, __FILE__);

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;

	hPipe = CreateNamedPipeA(("\\\\.\\pipe\\" + settings::inst().pipe_name.value).c_str(),
		PIPE_ACCESS_DUPLEX |
		FILE_FLAG_WRITE_THROUGH |
		FILE_FLAG_FIRST_PIPE_INSTANCE,
		PIPE_TYPE_BYTE |
		PIPE_READMODE_BYTE |
		PIPE_WAIT, 255, 128, 1024,
		60000, &sa);

	if (hPipe == INVALID_HANDLE_VALUE)
		throw base_exception(__LINE__, __func__, __FILE__);
}

void pipe::close() {
	LOGF;
	//write(pipe_codes::CLOSING);
	DisconnectNamedPipe(hPipe);
}

pipe::~pipe() {
	CloseHandle(hPipe);
}

void pipe::driver() {
	try {
		char buffer[1024];

		DWORD dwRead;

		LOGD("Pipe: waiting for connection...");

		if (ConnectNamedPipe(hPipe, nullptr)) // wait for someone to connect to the pipe
		{
			LOGD("Pipe connected...");
			while (true) {

				// Prima leggo l'opcode
				pipe_codes pc = read<pipe_codes>();
				LOGD("Ricevuto pipe-code: " << pc);
				switch(pc) {
				case pipe_codes::FILE_VERSION:
					{
						wstring fileName = read<wstring>();
						uint64_t timeStamp = read<uint64_t>();
							file_action fa = {0};
						fa.op_code = server::opcode::VERSION;
						fa.timestamps[5].dwLowDateTime = timeStamp;
						fa.timestamps[5].dwHighDateTime = timeStamp >> 32;
						action_merger::inst().add_change(fileName, fa);
					}
					break;
				case pipe_codes::WORK_COUNT:
					write<pipe_codes>(pipe_codes::WORK_COUNT);
					write<int32_t>(action_merger::inst().pending_count);
					break;
				default:
					//LOGD("Pipe: opcode not recognized. [" << (int)pc << "]");
					;
				}
			}
		}
		DWORD err = GetLastError();
		LOGD(err);
		throw base_exception(err, __LINE__, __func__, __FILE__);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

template<>
wstring pipe::read<wstring>() {
	LOGF;
	size_t size = read<uint32_t>();
	DWORD dwrr;
	LOGD(size);
	wstring ret;
	wchar_t *buff = new wchar_t[size+2]{0};
	//rr = recv(buff, size);
	if(!::ReadFile(hPipe, reinterpret_cast<char*>(buff), size*sizeof(wchar_t), &dwrr, nullptr) || size*sizeof(wchar_t) != dwrr){
		DWORD err = GetLastError();
		if (err != ERROR_BROKEN_PIPE)
			throw utilities::base_exception(err,__LINE__, __func__, __FILE__);
	}

	ret = wstring(buff, size);

	delete[] buff;
	return ret;
}

auth_exception::auth_exception(int l, const char* f, const char* ff) :
		base_exception("Wrong username/password", l,f,ff) {
	action_merger::inst().wait_time = settings::inst().max_waiting_time.value;
	pipe::inst().write(WRONG_CREDENTIALS);
}
