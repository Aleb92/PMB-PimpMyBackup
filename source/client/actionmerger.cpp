/*
 * actionmerger.cpp
 *
 *  Created on: 30 ott 2015
 *      Author: alessio
 */

#include <functional>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include <server/include/protocol.hpp>
#include "actionmerger.h"

using namespace std;
using namespace client;

inline static int get_flag_bit(DWORD event) {//FIXME
	switch (event) {
		case FILE_NOTIFY_CHANGE_FILE_NAME:
		case FILE_NOTIFY_CHANGE_DIR_NAME:
			return OP_MOVE;
		case FILE_NOTIFY_CHANGE_ATTRIBUTES:
		case FILE_NOTIFY_CHANGE_SECURITY:
			return OP_CHMOD;
		case FILE_NOTIFY_CHANGE_SIZE:
			return OP_WRITE;
		case FILE_NOTIFY_CHANGE_LAST_WRITE:
			return OP_TIMESTAMP | OP_WRITE;
		default:
			return 0;
	}
}

void action_merger::add_change(const wchar_t*filename, DWORD event,
		const FILETIME& f, const wchar_t*newName) {
	file_action& fa = map[filename];
	char flag = get_flag_bit(event);
	fa.op_code |= 1 << flag;
	if (flag == OP_MOVE_BIT) {
		if (newName)
			fa.newName = newName;
		else
			fa.op_code ^= OP_MOVE;
	}
	fa.timestamps[flag] = f;
}

action_merger::iterator action_merger::remove(action_merger::const_iterator i) {
	return map.erase(i);
}

void action_merger::remove(const wchar_t* i) {
	map.erase(i);
}
