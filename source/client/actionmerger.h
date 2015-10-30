#ifndef SOURCE_CLIENT_ACTIONMERGER_H_
#define SOURCE_CLIENT_ACTIONMERGER_H_

#include <functional>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <Windows.h>

#define DWORD int32_t
#define LPTSTR wchar_t*

typedef struct _FILETIME {
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME, *PFILETIME;

namespace client {

struct file_action {
	uint8_t op_code;
	FILETIME timestamps[8];
	std::wstring newName;
};

class action_merger {
	using maptype = std::unordered_map<std::wstring, file_action>;
	maptype map;
public:

	typedef maptype::iterator iterator;
	typedef maptype::const_iterator const_iterator;

	action_merger() = default;
	virtual ~action_merger() = default;

	void add_change(const wchar_t*, DWORD, const FILETIME&, const LPTSTR newName = nullptr);
	iterator remove(const_iterator);
	void remove(const wchar_t*);

	inline iterator begin() { return map.begin(); }
	inline const_iterator begin() const { return map.begin(); }
	inline iterator end() { return map.end(); }
	inline const_iterator end() const { return map.end(); }
};

}
#endif
