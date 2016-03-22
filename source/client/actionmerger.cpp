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
#include <limits>

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

/*
 * Since we read and write simultaneusly from two different threads and we iterate on the map we don't
 * want a reash to happen while we
 */
action_merger::action_merger(size_t estimatedFileNum) :
		map(estimatedFileNum) {
	map.max_load_factor(std::numeric_limits<float>::infinity());

	it = map.begin();
}

void action_merger::add_change(const change_entity& che) {

	//FIXME:
	if(che->Action == 0){
		map[L"*"].op_code = 0;
		return;
	}

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

	cv.notify_all();
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

bool action_merger::remove(std::wstring&, file_action&) {
	//TODO: Manino
	unique_lock<mutex> guard(lock);

	cv.wait(guard, [this]() {
		return !map.empty();
	});

	if (it == map.end())
		it = map.begin();

	pair<const wstring, const file_action> ret = make_pair(it->first,
			it->second);

	it = map.erase(it);
	return ret;
}
