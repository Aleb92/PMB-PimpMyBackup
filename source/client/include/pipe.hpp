
#ifndef SOURCE_CLIENT_INCLUDE_PIPE_HPP_
#define SOURCE_CLIENT_INCLUDE_PIPE_HPP_

#include <utilities/include/singleton.hpp>
#include <utilities/include/exceptions.hpp>
#include <mutex>

#include <Windows.h>

namespace client {

enum pipe_codes : uint8_t{
	WRONG_CREDENTIALS = 1,
	WORK_COUNT = 2,
    FILE_VERSION = 3
};

class auth_exception : public utilities::base_exception {
public:
	auth_exception(int l, const char * f, const char* ff);
};

class pipe: public utilities::singleton<pipe> {
	HANDLE hPipe;
	std::mutex lock;

	pipe();
	~pipe();
	friend class utilities::singleton<pipe>;
public:
	void driver();

	template<typename T>
	void write(T t) {
		DWORD written, err;
		if (!WriteFile(hPipe, &t, sizeof(t), &written, nullptr)) {
			err = GetLastError();
			if (err != ERROR_BROKEN_PIPE || written != sizeof(t))
				throw utilities::base_exception(err,__LINE__, __func__, __FILE__);
		}
	}

	template<typename T>
	void write(T* t, size_t l) {
		DWORD written, err;
		if (!WriteFile(hPipe, t, l, &written, nullptr)) {
			err = GetLastError();
			if (err != ERROR_BROKEN_PIPE || written != l)
				throw utilities::base_exception(err,__LINE__, __func__, __FILE__);
		}
	}

	template<typename T>
	T read() {
		DWORD dwRead;
		T ret;
		if (!::ReadFile(hPipe, (char*) &ret, sizeof(T), &dwRead, nullptr) ||
				dwRead != sizeof(T)) {
			DWORD err = GetLastError();
			if (err != ERROR_BROKEN_PIPE)
				throw utilities::base_exception(err,__LINE__, __func__, __FILE__);
		}
		return ret;
	}
};

	template<>
	std::wstring pipe::read<std::wstring>();
}

#endif /* SOURCE_CLIENT_INCLUDE_PIPE_HPP_ */
