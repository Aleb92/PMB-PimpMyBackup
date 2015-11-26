
#include <utilities/include/strings.hpp>
#include <utilities/include/fsutil.hpp>
#include <filesystem.hpp>
#include <settings.hpp>
#include <string>
#include <cstring>
#include <cwchar>
#include <istream>
#include <fstream>
#include <stack>
#include <windows.h>

using namespace std;
using namespace utilities;

namespace client {

file_info& directory::get(const wstring& file_name){
	wstring fn = file_name;
	return files.at(fn);
}

file_info directory::remove(const wstring& file_name){
	file_info oldFile = files.at(file_name);
	return oldFile;
}

file_info& directory::add(const wstring& file_name){
	return files[file_name];
}

static pair<wstring, wstring> file_dir_name(const wchar_t*name, size_t length = 0) {
	if(!length)
		length = wcslen(name);
	const wchar_t*file = wcsrchr(name, L'\\', length);
	if(file){
		++file;
		return make_pair(
			wstring(name, file - name),
			wstring(file, length - (file - name))
		);
	}
	else
		return make_pair(
			wstring(),
			wstring(file, length)
		);
}

file_info& filesystem::get_file(const wchar_t*name, size_t length) {
	auto name_info = file_dir_name(name, length);
	return directories.at(name_info.first).files[name_info.second];
}

directory& filesystem::get_dir(const wchar_t*name, size_t length) {
	if(length == 0)
		length = wcslen(name);
	return directories.at(wstring(name, length));
}

void filesystem::delete_dir(const wchar_t*name, size_t length) {
	if(length == 0)
		length = wcslen(name);
	directories.erase(wstring(name, length));
}

void filesystem::delete_file(const wchar_t*name, size_t length) {
	auto name_info = file_dir_name(name, length);
	directories.at(name_info.first).remove(name_info.second);
}

void filesystem::move_dir(const wchar_t*old, const wchar_t*_new) {
	auto oldName =file_dir_name(old), newName = file_dir_name(_new);
	//TODO
}

void filesystem::move_file(const wchar_t*n1, size_t l1 , const wchar_t*n2, size_t l2) {
	auto f1 = file_dir_name(n1, l1), f2 = file_dir_name(n2, l1);
	directory d1 = directories.at(f1.first);
	directories.at(f2.first).add(f2.second) = d1.get(f1.second);
	d1.remove(f1.second);
}

file_info& filesystem::new_file(const wchar_t*name, size_t length) {
	auto name_info = file_dir_name(name, length);
	return directories.at(name_info.first).get(name_info.second);
}

directory& filesystem::new_dir(const wchar_t*name, size_t length) {
	auto name_info = file_dir_name(name, length);
	directory& ret = directories[wstring(name, length)];
	directories[name_info.first].dirList[name_info.second] = ret;
	return ret;
}

const directory& filesystem::root() const {
	return directories[""];
}

struct dir_struct {
	size_t name_len, n_child;
};

struct file_struct {
	size_t name_len;
	file_info file;
};

filesystem::filesystem(){
	// Provo ad aprire il file
	ifstream file(settings::inst().tree_filename, ios::binary | ios_base::in);

	if(file.is_open()) {
		fromFile = true;
		wchar_t name[300];//Melius abundare quam deficere
		// Ottengo quanti elementi ho
		//Leggo quanti elementi ho
		size_t n, len;
		// Prendo il numero di directories
		file.read(reinterpret_cast<char*>(&n), sizeof(n));
		while(n) {
			// Prendo la dimensione del path
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			//Carico il path
			file.read(reinterpret_cast<char*>(name), len);
			//Creo la directory
			new_dir(name, len);
			--n;
		}

		// Ora i files:
		file.read(reinterpret_cast<char*>(&n), sizeof(n));
		while(n) {
			// Prendo la dimensione del path
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			//Carico il path
			file.read(reinterpret_cast<char*>(name), len);
			// Ora carico le informazioni del file
			file_info &f = new_file(name, len);
			file.read(reinterpret_cast<char*>(&f), sizeof(file_info));
			--n;
		}
		//fait!
	}
	else {
		fromFile = false;
		loadFS(settings::inst().watched_dir, L"");
	}
}

/**
 * Ben lontano dall'essere ottimale... ma comunque amen.
 * @param name
 * @param prefix
 */
void filesystem::loadFS(const wstring& name, const wstring& prefix) {
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFileW((name + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
        if (file_data.cFileName == '.')
            continue;

        const wstring file_name = file_data.cFileName;
        const wstring full_file_name = prefix + L"/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;


        if (is_directory) {
        	new_dir(full_file_name.c_str(), full_file_name.length());
        	loadFS(name + L"/" + file_name, full_file_name);
        }
        else {
        	// TOH un file!
        	file_info& fi = new_file(full_file_name.c_str(), full_file_name.length());
        	// Carico i dati!
        	fi.lastModified = file_data.ftLastWriteTime;
        	fi.mod = file_mod(file_data.dwFileAttributes);
        	fileMD5((name + L"/" + file_name).c_str(), fi.checksum);
        }

    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
}

filesystem::~filesystem() {

	size_t dirsNumber = directories.size(), filesNumber = 0;
	ofstream fileo(settings::inst().tree_filename, ios::binary | ios_base::out);

	fileo.write(reinterpret_cast<char*>(&dirsNumber), sizeof(size_t));

}

} /* namespace client */
