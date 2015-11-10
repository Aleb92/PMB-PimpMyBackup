
#include <utilities/include/strings.hpp>
#include <filesystem.hpp>
#include <string>

using namespace std;

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

filesystem::filesystem(HANDLE) {
	//TODO: GESU' AIUTACI TU'
	//FIXME
}

static pair<wstring, wstring> file_dir_name(const wchar_t*name, size_t length) {
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

void filesystem::move_dir(const wchar_t*, const wchar_t*) {
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
	return directories[wstring(name, length)];
}

} /* namespace client */
