
#include <directorylistener.hpp>

#include <iostream>
#include <iterator>

using namespace std;
using namespace client;

static void print(change_entity entity){
#define SW_ITEM(x) case x: wcout << L"" #x ": "; break

	switch(entity->Action) {
		SW_ITEM(FILE_ACTION_ADDED);
		SW_ITEM(FILE_ACTION_REMOVED);
		SW_ITEM(FILE_ACTION_MODIFIED);
		SW_ITEM(FILE_ACTION_RENAMED_OLD_NAME);
		SW_ITEM(FILE_ACTION_RENAMED_NEW_NAME);
	default:
		wcout << "Unknown (" << entity->Action << ")";
	}
#undef SW_ITEM
	copy(entity->FileName, entity->FileName + entity->FileNameLength/sizeof(wchar_t), ostream_iterator<char>(cout));
	wcout << endl;
}

int main() {
	wcout << "Inizializzo...";
	wcout.flush();
	directory_listener dl(L"PMB_test_root\\");
	wcout << "Fatto!" << endl;

	int count = 10;
	try {
		dl.scan<print>([&count](){ return ((--count) >= 0); });
	}
	catch (int ex) {
		return ex;
	}
}



