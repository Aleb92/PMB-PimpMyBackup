#include <threadpool.hpp>

#include <thread>
#include <chrono>

using namespace utilities;

using namespace std;

void thread_pool::join_all() {

	LOGF;

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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	cout << "end" << __func__ << endl;
}
