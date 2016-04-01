/*
 * atbegin.hpp
 *
 *  Created on: 05/ott/2015
 *      Author: Marco
 */

#ifndef ATBEGIN_HPP_
#define ATBEGIN_HPP_

#include <iostream>

namespace utilities {

	/**
	 *  Questa è una classe helper. Serve per iniettare codice prima dell'esecuzione del main,
	 * utile per inizializzare classi, inizializzare librerie e per registrare oggetti.
	 *  Riceve due argomenti template: il tipo di ritorno della funzione (per poterla instanziare)
	 * e la funzione stessa(un puntatore ad essa).
	 * Il costruttore (privato) richiama la funzione passata (ignorando il valore restituito).
	 *  Per poter chiamare il costruttore (e quindi la funzione) prima del main si sfrutta
	 * l'inizializzazione delle variabili globali e statiche. Questa classe infatti contiene
	 * una unica istanza (privata!) della classe. Un buon compilatore dovrebbe rimuovere
	 * completamente i rifierimenti inutilizzati a queste classi, linkare staticamente
	 * la funzione chiamata o addirittura (quando possibile) eseguirne l'inlining, per
	 * rendere il codice più veloce.
	 * Da utilizzare tramite atBegin
	 *
	 * @see at_end @see atBegin
	 */
	template <typename T, T (*func)()>
	class at_begin {
	private:
		at_begin() {
			(void)func();
		}
		static const at_begin<T, func> caller;
	};

	template <typename T, T (*func)()> const at_begin<T, func> at_begin<T, func>::caller;

	// Specializzazioni parziali di convenienza.
	template<bool (*func)()>
	class at_begin<bool, func> {
	private:
		at_begin() {
			if(!func())
				exit(EXIT_FAILURE);
		}
		static const at_begin<bool, func> caller;
	};

	template <bool (*func)()> const at_begin<bool, func> at_begin<bool, func>::caller;

	template<int (*func)()>
	class at_begin<int, func> {
	private:
		at_begin() {
			int ret = func();
			// Questo potrebbe dare problemi, ma tanto anche se cout non è ancora stato inizializzato,
			//non importa perchè stiamo già crashando... al massimo l'utente non vede questo messaggio..
			if(ret != 0){
				std::cerr << "Errore numero " << ret << " durante la chiamata a " << (void*)func << std::endl;
				exit(ret);
			}
		}
		static const at_begin<int, func> caller;
	};

	template<int (*func)()> const at_begin<int, func> at_begin<int, func>::caller;


#define atBegin(func) template class utilities::at_begin<decltype(func()), func>

}

#endif /* ATBEGIN_HPP_ */
