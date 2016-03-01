#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <client/include/directorylistener.hpp>
#include <utilities/include/fsutil.hpp>

#include <thread>
#include <deque>
#include <cstdio>
#include <string>]]
#include <mutex>
#include <locale>
#include <codecvt>
#include <Windows.h>

using namespace std;
using namespace client;
using namespace utilities;

#define DIR_TEST_FOLDER L"PMB_test\\PMB_root\\"

BOOST_AUTO_TEST_SUITE(windows_test)

struct fixture {
	mutex lock;
	deque<pair<DWORD, const wchar_t*>> commands;
	directory_listener dl;
	bool closed;

	fixture() :
			dl(DIR_TEST_FOLDER) {
		closed = false;
	}

	virtual ~fixture() {
		_wsystem(L"rmdir " DIR_TEST_FOLDER " /s /q");
	}

	void check(const change_entity ce) {
		lock_guard<mutex> lk(lock);
		BOOST_REQUIRE(!commands.empty());
		BOOST_CHECK_EQUAL(ce->Action, commands.front().first);

		//PRIMA VOLTA CHE BOOST NON FA QUEL CHE DEVE NELLA MIA ESPERIENZA!!!
		// Per far andare i test devo trasformare in utf8... :(
		wstring_convert<codecvt_utf8<wchar_t>> converter;

		string is = converter.to_bytes(ce->FileName, ce->FileName + ce->FileNameLength / sizeof(wchar_t)),
				should = converter.to_bytes(commands.front().second);

		commands.pop_front();

		BOOST_CHECK_EQUAL(is, should);

		if(closed && commands.size() == 0)
			dl.stop();
	}

	void log(DWORD act, const wchar_t*str) {
		lock_guard<mutex> lk(lock);
		if (!closed)
			commands.push_back(make_pair(act, str));
	}

	void close() {
		lock_guard<mutex> lk(lock);
		closed = true;
		if(commands.size() == 0)
			dl.stop();
	}
};

static void dcl_script(fixture* fix) {
	fix->log(FILE_ACTION_ADDED, L"prova.txt");
	fix->log(FILE_ACTION_MODIFIED, L"prova.txt");
	_wsystem(L"echo prova > " DIR_TEST_FOLDER L"\\prova.txt");

	fix->log(FILE_ACTION_MODIFIED, L"prova.txt");
	_wsystem(L"echo prova >> " DIR_TEST_FOLDER L"\\prova.txt");

	fix->log(FILE_ACTION_ADDED, L"prova");
	_wsystem(L"mkdir " DIR_TEST_FOLDER L"\\prova");

	fix->log(FILE_ACTION_ADDED, L"prova\\prova");
	fix->log(FILE_ACTION_MODIFIED, L"prova\\prova");
	_wsystem(L"echo prova > " DIR_TEST_FOLDER "\\prova\\prova");

	fix->log(FILE_ACTION_ADDED, L"prova\\kkp");
	fix->log(FILE_ACTION_MODIFIED, L"prova");
	fix->close();
	_wsystem(L"mkdir " DIR_TEST_FOLDER L"prova\\kkp");

}

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
	try {
		thread new_th(dcl_script, this);

		dl.scan<fixture, &fixture::check>(this);

		if (new_th.joinable())
			new_th.join();

	} catch (int e) {
		BOOST_ERROR("Errore apertura directory quando gia' creata...");
	}

}

BOOST_AUTO_TEST_SUITE_END()

#endif
