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

void action_merger::add_change(const wchar_t*filename, DWORD flags, const FILETIME& f,
		const wchar_t * newName = nullptr) {
	file_action& fa = map[filename];
	char flag = 0; // TODO
	fa.op_code |= 1<<flag;
	if((1<<flag) == MOVE) {
		if(newName)
			fa.newName = newName;
		else
			fa.op_code ^= MOVE;
	}
	fa.timestamps[flag] = f;
}

action_merger::iterator action_merger::remove(action_merger::const_iterator i) {
	return map.erase(i);
}

void action_merger::remove(const wchar_t* i) {
	map.erase(i);
}
