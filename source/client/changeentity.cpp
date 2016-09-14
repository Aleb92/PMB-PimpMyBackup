#include <directorylistener.hpp>
#include <Windows.h>
#include <memory>

using namespace std;
using namespace client;



change_entity::change_entity(std::shared_ptr<char>&whole,
		FILE_NOTIFY_INFORMATION*chunk, DWORD f) :
		container(whole), data(chunk), flags(f){
	::GetSystemTimeAsFileTime(const_cast<FILETIME*>(&time));
}


FILE_NOTIFY_INFORMATION& change_entity::operator*() {
	return *data;
}

FILE_NOTIFY_INFORMATION* change_entity::operator->() {
	return data;
}


const FILE_NOTIFY_INFORMATION& change_entity::operator*() const {
	return *data;
}

const FILE_NOTIFY_INFORMATION* change_entity::operator->() const {
	return data;
}

std::wostream& client::operator<< (std::wostream& out, const change_entity ce){
	out << ce->FileNameLength << L" ";
	return out.write(ce->FileName, ce->FileNameLength) << L" " << ce->Action << endl;
}
