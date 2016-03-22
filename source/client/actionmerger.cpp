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

void action_merger::add_change(const change_entity& che) {

	file_action& fa = map[wstring(che->FileName, che->FileNameLength)];
	opcode flag = get_flag_bit(che->Action);
	fa.op_code |= flag;

	if (che->Action == FILE_ACTION_RENAMED_OLD_NAME) {
		change_entity newNameEntity;
		if ((newNameEntity =
				utilities::shared_queue<change_entity>::inst().dequeue())->Action
				!= FILE_ACTION_RENAMED_NEW_NAME)
			//TODO ECCEZIONE
			throw errno;
		log::inst().issue(newNameEntity);
		fa.newName = wstring(newNameEntity->FileName,
				newNameEntity->FileNameLength);
	}

	// __builtin_ffs restituisce la posizione del primo bit meno significativo a 1
	// FIXME: sto indicizzando bene? (si)
	fa.timestamps[__builtin_ffs(flag) - 1] = che.time;
}

file_action& file_action::operator |=(const log_entry_header& entry) {
	op_code |= entry.op_code;
	FILETIME& time = timestamps[__builtin_ffs(entry.op_code) - 1];
	if (CompareFileTime(&entry.timestamp, &time) == 1)
		time = entry.timestamp;
	return *this;
}

file_action& file_action::operator ^=(const log_entry_header& entry) {
	for (uint8_t i = 0, j = 1; i < 8; i++, j << 1) {
		if (op_code & j) {
			FILETIME& _t = timestamps[i];
			if (CompareFileTime(&entry.timestamp, &_t) == 1) {
				_t = {0};
				opcode ^= j;
			}
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
