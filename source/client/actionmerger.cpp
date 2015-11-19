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
#include <actionmerger.hpp>

using namespace std;
using namespace client;
using namespace server;


inline static opcode get_flag_bit(DWORD event) {
	switch (event) {
		case FILE_ACTION_ADDED:
			return CREATE;
		case FILE_ACTION_REMOVED:
			return REMOVE;
		case FILE_ACTION_MODIFIED:
			return WRITE;
		case FILE_ACTION_RENAMED_OLD_NAME:
		case FILE_ACTION_RENAMED_NEW_NAME:
			return MOVE;
	}
	return INVALID;
}

void action_merger::add_change(const wchar_t*filename, DWORD event,
		const FILETIME& f, const wchar_t*newName) {
	file_action& fa = map[filename];
	opcode flag = get_flag_bit(event);
	fa.op_code |= flag;
	if (flag == MOVE) {
		if (newName)
			fa.newName = newName;
		else
			fa.op_code ^= opcode::MOVE;
	}
	fa.timestamps[__builtin_ffs(flag) - 1] = f;
}

action_merger::iterator action_merger::remove(action_merger::const_iterator i) {
	return map.erase(i);
}

void action_merger::remove(const wchar_t* i) {
	map.erase(i);
}
