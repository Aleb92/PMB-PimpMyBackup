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


inline opcode client::get_flag_bit(DWORD event) {
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
	// __builtin_ffs restituisce in pratica log2(x) + 1
	// FIXME: sto indicizzando bene?
	fa.timestamps[__builtin_ffs(flag) - 1] = f;
}

file_action& file_action::operator |=(const log_entry_header& entry){
	op_code |= entry.op_code;
	FILETIME& time = timestamps[__builtin_ffs(entry.op_code) - 1];
	if(CompareFileTime(&entry.timestamp, &time) == 1)
		time = entry.timestamp;
	return *this;
}

file_action& file_action::operator ^=(const log_entry_header& entry) {
	//TODO
	for(int i = 0, j = 1; i < 8; i++, j << 1){
		FILETIME& _t = timestamps[i];
		if(CompareFileTime(&entry.timestamp, &_t) == 1){

		}
	}
	return *this;
}

action_merger::iterator action_merger::remove(action_merger::const_iterator i) {
	return map.erase(i);
}

void action_merger::remove(const wchar_t* i) {
	map.erase(i);
}
