#include <server/include/protocol.hpp>
#include <actionmerger.hpp>
#include <log.hpp>

#include <functional>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include <limits>

using namespace std;
using namespace client;
using namespace server;

opcode client::get_flag_bit(DWORD event) {
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

action_merger::action_merger(size_t estimatedFileNum) :
		map(estimatedFileNum), open(true) {

	// Qui impedisco il rehash per mantenere gli iteratori validi.
	map.max_load_factor(std::numeric_limits<float>::infinity());

	it = map.begin();
}

void action_merger::add_change(std::wstring& fileName, file_action& action) {
	if(open) {
		if(map.count(fileName)) {
			file_action& up = map[fileName];
			up.op_code |= action.op_code;
			for(int i = 0; i < 8; i++)
				if (CompareFileTime(&action.timestamps[i], &up.timestamps[i]) == 1)
						up.timestamps[i] = action.timestamps[i];
			//FIXME: è possibile avere un rename quì? Secondo me non dovrebbe...
		}
		else
			map[fileName] = action;
	}
}

void action_merger::add_change(const change_entity& che) {
	if(open) {
		file_action& fa = map[wstring(che->FileName, che->FileNameLength)];
		opcode flag = get_flag_bit(che->Action);
		fa.op_code |= flag;

		if (che->Action == FILE_ACTION_RENAMED_OLD_NAME) {
			change_entity newNameEntity =
					utilities::shared_queue<change_entity>::inst().dequeue();
			if (newNameEntity->Action
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
	// FIXME: qui devo lanciare una eccezione?
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
				op_code ^= j;
			}
		}
	}
	return *this;
}

bool action_merger::remove(std::wstring& name, file_action& actions) {
	unique_lock<mutex> guard(lock);

	// Per poter rimuovere qualcosa devo sempre aver qualcosa da rimuovere..
	cv.wait(guard, [this]() {
		return !map.empty() || (!open);
	});

	// Se stiamo chiudendo mi fermo...
	if (map.empty())
		return false;

	// Next up
	if (it == map.end())
		it = map.begin();

	// Ora prelevo le informazioni perchè sono sicuro che il mio iteratore
	// sia valido (no rehash + size > 0 + no end())

	name = it->first;
	actions = it->second;

	// Next one
	it = map.erase(it);
	return true;
}

void action_merger::close() {
	lock_guard<mutex> guard(lock);
	open = false;
	cv.notify_all();
}
