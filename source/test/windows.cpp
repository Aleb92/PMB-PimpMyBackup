#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <client/include/directorylistener.hpp>

#include <thread>
#include <cstdio>
#include <Windows.h>

using namespace std;
using namespace client;

BOOST_AUTO_TEST_SUITE(windows_test)

bool stopper(){
	static int a = 4;
	return a--;
}

void print(change_entity entity){
	wprintf(entity->FileName);
}

void create_file_1(){
	system("type NUL > PMB_test_root\\file_prova.txt");
}

BOOST_AUTO_TEST_CASE(create_directory_recursively){
	try{
		{
			directory_listener(L"PMB_test\\PMB_root\\");

		}
		system("rmdir PMB_test\\PMB_root /q");
		system("rmdir PMB_test /q");
	}catch(...){
		BOOST_ERROR( "Errore creazione directory ricorsivamente..." );
	}
}
/*
BOOST_AUTO_TEST_CASE(directory_change_listener){

	system("mkdir PMB_test_root\\");
	try{
		directory_listener dl("PMB_test_root\\");
		dl.scan<print>(stopper);
		thread new_th(create_file_1);
		new_th.join();
		system("del /q PMB_test_root\\*");
		system("rmdir PMB_test_root /q");

	}catch (int e){
		BOOST_ERROR( "Errore apertura directory quando gia' creata..." );
	}
}*/

BOOST_AUTO_TEST_SUITE_END()

#endif
