#include <settings.hpp>
#include <log.hpp>
#include <utilities/include/exceptions.hpp>
#include <utilities/include/debug.hpp>
#include <utilities/include/strings.hpp>

#include <windows.h>
#include <io.h>
#include <cstdio>
#include <unordered_map>

using namespace std;
using namespace client;
using namespace utilities;

log::log() {

	LOGF;
	wstring oldfile_name = settings::inst().log_filename.value + L".old";
	unordered_map<wstring, file_action> load_map;

	if (MoveFileW(settings::inst().log_filename.value.c_str(),
					oldfile_name.c_str())) {

		LOGD("Rinominato file di LOG");

		FILE* old_log = _wfopen(oldfile_name.c_str(), L"rb");
		if (old_log == NULL)
		throw fs_exception("No old log",__LINE__, __func__, __FILE__);

		log_entry_header entry;

		while (!feof(old_log)) {
			if(fread(&entry, sizeof(struct log_entry_header), 1, old_log) == 0)
			continue;

			wstring entityName;
			entityName.resize(entry.length / sizeof(wchar_t));
			if(fread(&entityName[0], 1, entry.length, old_log) != entry.length) {
				cerr << "File log corrotto-name" << endl;
				continue;
			}

			file_action& eInfo = load_map[entityName];

			if (entry.type == 'i') {
				eInfo |= entry;

				LOGD("Entry type i -" << utf8_encode(entityName));

				if (entry.op_code & server::opcode::MOVE) {
					size_t length;
					fread(&length, sizeof(size_t), 1, old_log);
					eInfo.newName.resize(length / sizeof(wchar_t));
					fread(&eInfo.newName[0], length, 1, old_log);
					LOGD("Riconosciuta una move con newName: "<<utf8_encode(eInfo.newName));
				}
			} else {
				LOGD("Entry type c -" << utf8_encode(entityName));
				eInfo ^= entry;

				if (eInfo.op_code == 0) {
					LOGD(utf8_encode(entityName) << " done.");
					load_map.erase(entityName);
				}
			}
		}
		fclose(old_log);
		DeleteFileW(oldfile_name.c_str());
	}

	HANDLE file_handle = CreateFileW(
			settings::inst().log_filename.value.c_str(),
			GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS,
			FILE_FLAG_WRITE_THROUGH, NULL);

	if (file_handle != INVALID_HANDLE_VALUE) {

		LOGD("Creato file di log");

		int file_descriptor = _open_osfhandle((intptr_t) file_handle, 0);

		if (file_descriptor != -1) {

			log_file = _fdopen(file_descriptor, "wb");

			if (log_file == NULL) {
				throw fs_exception("no new log",__LINE__, __func__, __FILE__);
			}

			LOGD("Aperto file di log");

			for(auto& action : load_map) {
				LOGD("Logging old entry: " << utf8_encode(action.first));
				issue(action.second, action.first);
			}

			/*
			 * Riempe la action merger con le nuove entry ancora da completare trovate nella scansione
			 * del vecchio log
			 */
			action_merger::inst().map.swap(load_map);
		}
	}
}

void log::issue(const change_entity& entity) {

	LOGF;

	if (entity->Action == FILE_ACTION_RENAMED_NEW_NAME) {
		size_t length = static_cast<size_t>(entity->FileNameLength);
		fwrite(&length, sizeof(size_t), 1, log_file);
		fwrite(entity->FileName, length, 1, log_file);
	} else {
		log_entry_header h = {'i', get_flag_bit(entity->Action, entity.flags), entity.time,
			static_cast<size_t>(entity->FileNameLength)};

		fwrite(&h, sizeof(struct log_entry_header), 1, log_file);
		fwrite(entity->FileName, h.length, 1, log_file);
	}
	fflush(log_file);
}

void log::finalize(const file_action& action, const wstring& name) {

	LOGF;

	log_entry_header h = {'c', static_cast<server::opcode>(action.op_code),
		action.timestamps[0], name.length() * sizeof(wchar_t)};

	for (int i = 1; i < 8; i++)
	if (CompareFileTime(action.timestamps + i, &h.timestamp) > 0)
	h.timestamp = action.timestamps[i];

	fwrite(&h, sizeof(struct log_entry_header), 1, log_file);
	fwrite(name.c_str(), h.length, 1, log_file);

	fflush(log_file);
}

void log::issue(const file_action& action, const std::wstring& name) {

	LOGF;
	log_entry_header h = {'i', static_cast<server::opcode>(action.op_code),
		action.timestamps[0], name.length() * sizeof(wchar_t)};

	for (int i = 1; i < 8; i++)
	if (CompareFileTime(action.timestamps + i, &h.timestamp) > 0)
	h.timestamp = action.timestamps[i];

	fwrite(&h, sizeof(struct log_entry_header), 1, log_file);
	fwrite(name.c_str(), sizeof(wchar_t), name.length(), log_file);

	if(action.op_code & server::opcode::MOVE){
		size_t newNameSize = action.newName.length()*sizeof(wchar_t);
		fwrite(&newNameSize, sizeof(size_t), 1, log_file);
		fwrite(action.newName.c_str(), sizeof(wchar_t), action.newName.length(), log_file);
	}

	fflush(log_file);
}

log::~log() {
	fclose(log_file);
}
