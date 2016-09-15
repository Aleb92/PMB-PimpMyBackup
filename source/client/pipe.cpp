#include <settings.hpp>
#include <pipe.hpp>
#include <actionmerger.hpp>

#include <iostream>
#include <Windows.h>

using namespace client;
using namespace utilities;
using namespace std;

pipe::pipe() {
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

	hPipe = CreateNamedPipeA(settings::inst().pipe_name.value.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 128, 1024,
		NMPWAIT_WAIT_FOREVER, &sa);

	if (hPipe == INVALID_HANDLE_VALUE)
		throw base_exception(__LINE__, __func__, __FILE__);
}

pipe::~pipe() {
	//FIXME
	while (!lock.try_lock()) {
		// Interrompi l'attesa!
		CancelIoEx(hPipe, NULL);
		this_thread::yield();
	}
	CloseHandle(hPipe);
	lock.unlock();
}

void pipe::driver() {
	try {
		char buffer[1024];

		DWORD dwRead;
		unique_lock<mutex> guard(lock);

		LOGD("Pipw: waiting for connection...");
		//FIXME: Questo thread rimane bloccato quì e impedisce la corretta chiusura della pipe.
		while (ConnectNamedPipe(hPipe, nullptr)) // wait for someone to connect to the pipe
		{
			LOGD("Pipe connected...");
			{
				guard.unlock();
				on_return<> disconnect([this]() {
					DisconnectNamedPipe(this->hPipe);
					LOGD("Pipe disconnected.");
				});

				guard.lock();
				while (true) {
					guard.unlock();

					// Prima leggo l'opcode
					pipe_codes pc = read<pipe_codes>();
					switch(pc) {
					case pipe_codes::FILE_VERSION:
						{
							wstring fileName = read<wstring>();
							uint64_t timeStamp = read<uint64_t>();

							file_action fa;
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
						LOGD("Pipe: opcode not recognized. [" << (int)pc << "]");
					}

					guard.lock();
				}
			}
			guard.unlock();
			DWORD err = GetLastError();
			if (err != ERROR_BROKEN_PIPE)
				throw base_exception(err,__LINE__, __func__, __FILE__);
			guard.lock();

		}
		throw base_exception(__LINE__, __func__, __FILE__);
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
	if(!::ReadFile(hPipe, reinterpret_cast<char*>(buff), size*sizeof(wchar_t), &dwrr, nullptr) || size != dwrr){
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
