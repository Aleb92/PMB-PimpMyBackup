#define BOOST_TEST_DYN_LINK
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

		ss.send(ss.recv<int>()*ss.recv<int>());
		char str[10];
		ss.recv(str);
		ss.send("ciao");
	}
	catch (...) { }
}

void client() {
	try {
		socket_stream ss("127.0.0.1", DEFAULT_PORT);
		ss.send(10);
		ss.send(44);
		ss.recv<int>();
		ss.send("bonnyculo");
		char pp[5];
		char *p = pp;
		ss.recv(p, 5);
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
	BOOST_CHECK_NO_THROW({
		socket_stream ss  = socket_stream("127.0.0.1", DEFAULT_PORT);
		ss.send(10);
		ss.send(44);;
		BOOST_CHECK_EQUAL(ss.recv<int>(), 440);
		ss.send("bonnycul0");
		char pp[5];
		char *p = pp;
		BOOST_CHECK_EQUAL(ss.recv(p, 5), 5);
		BOOST_CHECK_EQUAL(strcmp(p, "ciao"), 0);
	});
	//Adesso aspetto l'altro thread
	if(s.joinable()) s.join();
}

BOOST_AUTO_TEST_CASE(listener)
{
	socket_listener ls = socket_listener();


	// Faccio partire il client visto che sono pronto a ricevere
	thread c(client);

	BOOST_CHECK_NO_THROW({
		// Accetto connessioni
		socket_stream ss = ls.accept();

		// Test case
		BOOST_CHECK_EQUAL(ss.recv<int>(), 10);
		BOOST_CHECK_EQUAL(ss.recv<int>(), 44);
		ss.send(440);
		char str[10];
		BOOST_CHECK_EQUAL(ss.recv(str), sizeof(str));
		ss.send("ciao");
	});

	//Join per evitare problemi
	if(c.joinable()) c.join();
}

BOOST_AUTO_TEST_SUITE_END()
