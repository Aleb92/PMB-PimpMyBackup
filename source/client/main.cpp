
#include <settings.hpp>
#include <client.hpp>
#include <utilities/include/strings.hpp>

#include <iostream>
#include <iterator>

using namespace std;
using namespace client;

int main(int argc, char *argv[]) {

	if(argc > 1 && argv[1][1] == 'v') {
		//TODO scegliere una licenza e aggiungere nota et similia..
		cout << "Pimp My Backup Client Copyright (C) 2016  Alessio Bonato and Marco Manino \r\n\r\n"
		"This program comes with ABSOLUTELY NO WARRANTY; for details see "
		"<http://www.gnu.org/licenses/gpl.html>.\r\n"
		"This is free software, and you are welcome to redistribute it"
		"under certain conditions.\r\n";
		return 0;
	}

	/*
	 * Questo codice dovrebbe essere strutturato diversamente per diventare un servizio
	 * tuttavia (per ora) è il main  a gestire il tutto...
	 */
	client::client c; // più semplice di così...
	// Comando di synchronize (dovrebbe essere fatto su un altro thread per non bloccare
	// il service manager...)
	if(settings::inst().synchronize){
		if(!c.synchronize())
			return EXIT_FAILURE;
	}

	// Inizio
	c.start();

	// Per ora come segnale di stop, scrivo "q" nella shell
	char r;
	while((cin >> r))
		if(r == 'q' || r == 'Q')
			break;

	// E stoppo
	c.stop();
}

