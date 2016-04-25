#include <settings.hpp>
#include <client.hpp>
#include <service.hpp>

#include <utilities/include/strings.hpp>

#include <exception>
#include <iostream>
#include <iterator>

using namespace std;
using namespace client;

int main(int argc, char *argv[]) {
	try {
		if (argc > 1 && argv[1][0])
			switch (argv[1][1]) {
			case 'v':
				cout
						<< "Pimp My Backup Client version 0." PMB_VERSION " Copyright (C) 2016  Alessio Bonato and Marco Manino \r\n\r\n"
						"This program comes with ABSOLUTELY NO WARRANTY; for details see "
						"<http://www.gnu.org/licenses/gpl.html>.\r\n"
						"This is free software, and you are welcome to redistribute it"
						"under certain conditions.\r\n";
				return 0;
			case 's':
				// Se richiesto faccio partire il servizio
				start_service();
				return 0;
			default:
				cout << "Argument not recgnized...";
			}

		// Se no faccio quì in locale (per il debug)
		client::client c; // più semplice di così...

		// Inizio
		c.start();

		// Per ora come segnale di stop, scrivo "q" nella shell
		char r;
		while ((cin >> r))
			if (r == 'q' || r == 'Q')
				break;

		// E stoppo
		c.stop();
	} catch (const exception& ex) {
		cout << "Error: " << ex.what() << endl;
	} catch (...) {
		cout << "Unknown exception thrown" << endl;
	}
}

