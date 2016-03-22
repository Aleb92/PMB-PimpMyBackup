#ifndef SOURCE_UTILITIES_THREADPOOL_HPP_
#define SOURCE_UTILITIES_THREADPOOL_HPP_

#include <functional>
#include <future>
#include <thread>
#include <atomic>

namespace utilities {

class thread_pool {

	std::atomic<bool> running;
	std::deque<std::thread> waitingList;
	std::thread joiner;

public:

	thread_pool() :
			running(true), joiner(join_all, this) {

	}

	inline void stop(void) {
		running = false;
		if(joiner.joinable())
			joiner.join();
	}

	inline bool isRunning() {
		return running;
	}

	static void execute(std::function<void(std::atomic<bool>&)> f) {
		if (!running)
			return;
		waitingList.push_back(std::thread(f, running));
	}

	void join_all() {
		while (running) {
			while (!waitingList.empty)
				if (waitingList.front().joinable()) {
					waitingList.front().join();
					waitingList.pop_front();
				} else
					std::this_thread::yield();

		}
		std::this_thread::yield();
	}
};

}
#endif
