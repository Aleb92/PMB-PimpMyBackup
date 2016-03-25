#include <threadpool.hpp>

using namespace utilities;

void thread_pool::join_all() {
	// Questa variabile serve a far fare un ultimo giro per
	// assicurarsi che la join venga fatta su tutti i thread.
	bool last_round = true;
	while (running || last_round) {
		last_round = running;
		while (!waitingList.empty())
			if (waitingList.front().joinable()) {
				waitingList.front().join();
				waitingList.pop_front();
			} else
				std::this_thread::yield();
	}
	std::this_thread::yield();
}
