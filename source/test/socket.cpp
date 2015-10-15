#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Tests
#include <boost/test/unit_test.hpp>

#include <utilities/include/socket.hpp>

#include <mutex>
#include <thread>

using namespace std;
using namespace utilities;

mutex sem;

BOOST_AUTO_TEST_SUITE(socket_test)


void server() {
	try {
		socket_listener ls;

		sem.unlock();
		socket_stream ss = ls.accept();

		(void)ss.send(ss.recv<int>()*ss.recv<int>());
		char str[10];
		(void)ss.recv(str);
		(void)ss.send("ciao");
	}
	catch (...) { }
}

void client() {
	try {
		socket_stream ss("127.0.0.1", DEFAULT_PORT);
		ss.send(10);
		ss.send(44);
		(void)ss.recv<int>();
		ss.send("bonnyculo");
		char pp[5];
		char *p = pp;
		(void)ss.recv(p, 5);
	}
	catch (...) {}
}

BOOST_AUTO_TEST_CASE(stream)
{
	// Uso un mutex come semaforo binario e lo metto a 0.
	sem.lock();
	//Faccio partire il server
	thread s(server);

	//Aspetto che il server abbia fatto bind e sia attivo...
	lock_guard<mutex> gd(sem);

	// Divento io stesso il client
	{
		socket_stream ss  = socket_stream("127.0.0.1", DEFAULT_PORT);
		BOOST_CHECK(ss.send(10) == sizeof(10));
		BOOST_CHECK(ss.send(44) == sizeof(44));
		BOOST_CHECK(ss.recv<int>() == 440);
		BOOST_CHECK(ss.send("bonnycul0") == 10);
		char pp[5];
		char *p = pp;
		BOOST_CHECK(ss.recv(p, 5) == 5);
		BOOST_CHECK(strcmp(p, "ciao") == 0);
	}
	//Adesso aspetto l'altro thread
	if(s.joinable()) s.join();
}

BOOST_AUTO_TEST_CASE(listener)
{
	socket_listener ls = socket_listener();

	// Faccio partire il client visto che sono pronto a ricevere
	thread c(client);
	// Accetto connessioni
	socket_stream ss = ls.accept();

	// Test case
	BOOST_CHECK(ss.recv<int>() == 10);
	BOOST_CHECK(ss.recv<int>() == 44);
	BOOST_CHECK(ss.send(440) == sizeof(440));
	char str[10];
	BOOST_CHECK(ss.recv(str) == sizeof(str));
	BOOST_CHECK(ss.send("ciao") == 5);

	//Join per evitare problemi
	if(c.joinable()) c.join();
}

BOOST_AUTO_TEST_SUITE_END()
