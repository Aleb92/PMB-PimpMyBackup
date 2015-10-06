#include "../utilities/socket.hpp"

#include <thread>
#include <iostream>

using namespace std;
using namespace utilities;

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
		perror("Server");
	}
}

void client() {
	try {
		cout << "Sono il client!! Mi connetto..." << endl;
		socket_stream ss(0, DEFAULT_PORT);
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
		perror("Client");
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
