#include <directorylistener.hpp>
#include <Windows.h>
#include <memory>

using namespace std;
using namespace client;


change_entity::change_entity(std::shared_ptr<char>&whole,
		FILE_NOTIFY_INFORMATION*chunk) :
		container(whole), data(chunk) {
}


FILE_NOTIFY_INFORMATION& change_entity::operator*() {
	return *data;
}

FILE_NOTIFY_INFORMATION* change_entity::operator->() {
	return data;
}
