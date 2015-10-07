#include "../utilities/socket.hpp"

#include <thread>
#include <iostream>

using namespace std;
using namespace utilities;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

# include <windows.h>

void wperror(const char* c, int errcode) {
	char *msg = nullptr;
	// OK ora chiedo la formattazione
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, errcode, 0, (LPTSTR)&msg, 128, nullptr)){
		cout << c << msg << endl;
	}
}
#else
#define wperror(c, code) perror(c)
#endif

void server() {
	try {
		cout << "Sono il server. Creo il listener..." << endl;

		socket_listener ls;
		cout << "Fatto!" << endl << "Ora accetto connessioni..." << endl;
		socket_stream ss = ls.accept(1);
		cout << "Fatto!" << endl;
		int a, b;
		a = ss.recv<int>();
		cout << a << endl;
		b = ss.recv<int>();
		cout << b << endl;
		ss.send(a*b);
		char str[10];
		cout << ss.recv(str) << ')' << str << endl;
		ss.send("ciao");
	}
	catch (int a) {
		wperror("Server", a);
	}
}

void client() {
	try {
		cout << "Sono il client!! Mi connetto..." << endl;

		socket_stream ss("127.0.0.1", DEFAULT_PORT);

		cout << "Fatto! Ora invio porcate!" << endl;
		ss.send(10);
		ss.send(44);
		cout << 'c' << ss.recv<int>() << endl;
		ss.send("bonnycul");
		char pp[5];
		char *p = pp;
		cout << 'c' << ss.recv(p, 5) << ')' << p << endl;
	}
	catch (int a) {
		wperror("Client", a);
	}

}

int main() {
	thread s(server);
	this_thread::sleep_for(chrono::seconds(2));
	thread c(client);

	if(s.joinable()) s.join();
	if(c.joinable()) c.join();
	return 0;
}
