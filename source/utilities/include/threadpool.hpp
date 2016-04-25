#ifndef SOURCE_UTILITIES_THREADPOOL_HPP_
#define SOURCE_UTILITIES_THREADPOOL_HPP_

#include <functional>
#include <deque>
#include <future>
#include <thread>
#include <atomic>

namespace utilities {

/**
 * Implementa una pool di thread. In questa semplice implementazione
 * ad ogni azione corrisponde la creazione di un nuovo thread.
 * Come funzionalità aggiuntiva è presente uno zombie che si assicura
 * la chiusura di tutti i thread e una funzione di stop per essere sicuri
 * che tutti i thread abbiamo finito di lavorare.
 */
class thread_pool {

	volatile bool running;
	std::deque<std::thread> waitingList;
	// Thread di attesa per terminare i vari thread aperti...
	// mi ricorda molto quello che fa init in un sistema unix...
	std::thread joiner;

public:

	/**
	 * metodo demone per fare join su tutti i thread e liberare risorse.
	 */
	void join_all();
	
	/**
	 * Inizializza una nuova pool
	 */
	inline thread_pool() :
			running(true), joiner(&thread_pool::join_all, this) { }

	/**
	 * Interrompe questa pool. Una volta interrotta una thread_pool
	 * non può più tornare operativa.
	 */
	inline void stop(void) {
		running = false;
		if(joiner.joinable())
			joiner.join();
	}

	/**
	 * @return Restituisce lo stato della thread_pool
	 */
	inline bool isRunning() {
		return running;
	}

	/**
	 * Esegue su un thread una funzione qualsiasi: accetta
	 * lambda, funzioni statiche, bindings di funzioni,
	 * metodi etc...
	 * @param f funzione
	 * @param args argomenti
	 */
    template<typename T, typename... A>
	void execute(T&& f, A&&... args) {
		if (!running)
			return;
		waitingList.push_back(std::thread(f, std::forward<A>(args)..., std::ref(running)));
	}

};

}
#endif
