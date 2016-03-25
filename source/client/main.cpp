#include <client.hpp>
#include <settings.hpp>

#include <iostream>
#include <iterator>

using namespace std;
using namespace client;

int main() {
	/*
	 * Questo codice dovrebbe essere strutturato diversamente per diventare un servizio
	 * tuttavia (per ora) è il main  a gestire il tutto...
	 */
	client::client c; // più semplice di così...
	// Comando di synchronize (dovrebbe essere fatto su un altro thread per non bloccare
	// il service manager...)
	if(client::settings::inst().synchronize){
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

