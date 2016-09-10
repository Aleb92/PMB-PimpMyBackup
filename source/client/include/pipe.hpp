#ifndef SOURCE_CLIENT_INCLUDE_PIPE_HPP_
#define SOURCE_CLIENT_INCLUDE_PIPE_HPP_

#include <utilities/include/singleton.hpp>
#include <utilities/include/exceptions.hpp>
#include <mutex>

#include <Windows.h>

namespace client {

enum pipe_codes {
	WRONG_CREDENTIALS = 1
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

};

}

#endif /* SOURCE_CLIENT_INCLUDE_PIPE_HPP_ */
