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


inline static opcode get_flag_bit(DWORD event) {//FIXME
	switch (event) {
		case FILE_NOTIFY_CHANGE_FILE_NAME:
		case FILE_NOTIFY_CHANGE_DIR_NAME:
			return opcode::REMOVE ;
		case FILE_NOTIFY_CHANGE_ATTRIBUTES:
		case FILE_NOTIFY_CHANGE_SECURITY:
			return opcode::CHMOD;
		case FILE_NOTIFY_CHANGE_SIZE:
			return opcode::WRITE;
		case FILE_NOTIFY_CHANGE_LAST_WRITE:
			return opcode::TIMESTAMP | opcode::WRITE;
		default:
			return opcode::LIST;
	}

}

void action_merger::add_change(const wchar_t*filename, DWORD event,
		const FILETIME& f, const wchar_t*newName) {
	file_action& fa = map[filename];
	opcode flag = get_flag_bit(event);
	fa.op_code |= flag;
	if (flag == opcode::MOVE) {
		if (newName)
			fa.newName = newName;
		else
			fa.op_code ^= opcode::MOVE;
	}
	// gcc only
	fa.timestamps[__builtin_ffs(flag) - 1] = f;
}

action_merger::iterator action_merger::remove(action_merger::const_iterator i) {
	return map.erase(i);
}

void action_merger::remove(const wchar_t* i) {
	map.erase(i);
}
