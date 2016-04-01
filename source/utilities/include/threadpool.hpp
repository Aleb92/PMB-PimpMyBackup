#ifndef SOURCE_UTILITIES_THREADPOOL_HPP_
#define SOURCE_UTILITIES_THREADPOOL_HPP_

#include <functional>
#include <deque>
#include <future>
#include <thread>
#include <atomic>

namespace utilities {

class thread_pool {

	volatile bool running;
	std::deque<std::thread> waitingList;
	// Thread di attesa per terminare i vari thread aperti...
	// mi ricorda molto quello che fa init in un sistema unix...
	std::thread joiner;

public:

	inline thread_pool() :
			running(true), joiner(join_all, this) { }

	inline void stop(void) {
		running = false;
		if(joiner.joinable())
			joiner.join();
	}

	inline bool isRunning() {
		return running;
	}

	// Alla fine, serve un template...
    template<typename T, typename... A>
	void execute(T&& f, A&&... args) {
		if (!running)
			return;
		waitingList.push_back(std::thread(f, std::forward<A>(args)..., std::ref(running)));
	}

	void join_all();
};

}
#endif
