#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <client/include/directorylistener.hpp>
#include <utilities/include/fsutil.hpp>

#include <thread>
#include <deque>
#include <cstdio>
#include <string>
#include <mutex>
#include <Windows.h>

using namespace std;
using namespace client;
using namespace utilities;

#define DIR_TEST_FOLDER L"PMB_test\\PMB_root\\"

BOOST_AUTO_TEST_SUITE(windows_test);

struct fixture {
	mutex lock;
	deque<pair<DWORD, const wchar_t*>> commands;
	directory_listener dl;
	bool closed;

	fixture() : dl(DIR_TEST_FOLDER) {
		closed = false;
	}

	virtual ~fixture() {
		_wsystem(L"rmdir " DIR_TEST_FOLDER " /s /q");
	}

	void check(const change_entity ce) {
		lock_guard<mutex> lk(lock);
		BOOST_ASSERT(!commands.empty());
		BOOST_CHECK_EQUAL(ce->Action, commands.front().first);
		BOOST_CHECK_EQUAL(wstring(ce->FileName, ce->FileNameLength).c_str(),
				commands.front().second);
		commands.pop_front();
	}

	void log(DWORD act, const wchar_t*str, bool last = false) {
		lock_guard<mutex> lk(lock);
		if(!closed) {
			commands.push_back(make_pair(act, str));
			closed = closed || last;
		}
	}

	bool keep() {
		return !closed || !commands.empty();
	}
};


static void dcl_script(fixture* fix) {
	fix->log(FILE_ACTION_ADDED, L"prova.txt");
	fix->log(FILE_ACTION_MODIFIED, L"prova.txt", true);
	_wsystem(L"echo prova >> " DIR_TEST_FOLDER L"\\prova.txt");
}

struct bah {
	int a;
};

BOOST_AUTO_TEST_CASE(create_directory_recursively) {
	try {
		{
			directory_listener(DIR_TEST_FOLDER);
		}
		BOOST_CHECK(pathExists(DIR_TEST_FOLDER));
		BOOST_CHECK(isPathDir(DIR_TEST_FOLDER));
		_wsystem(L"rmdir " DIR_TEST_FOLDER " /q");
		_wsystem(L"rmdir PMB_test /q");
	} catch (...) {
		BOOST_ERROR("Errore creazione directory ricorsivamente...");
	}
}

BOOST_FIXTURE_TEST_CASE(directory_change_listener, fixture) {
	_wsystem(L"mkdir " DIR_TEST_FOLDER);
	try {
		thread new_th(dcl_script, this);

		dl.scan([this](const change_entity ce) {this->check(ce);},
				[this]() {return this->keep();});

		if(new_th.joinable())
			new_th.join();

	} catch (int e) {
		BOOST_ERROR("Errore apertura directory quando gia' creata...");
	}

}

BOOST_AUTO_TEST_SUITE_END()

#endif
