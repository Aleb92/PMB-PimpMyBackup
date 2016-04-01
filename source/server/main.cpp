#include <settings.hpp>
#include <database.hpp>

#include <utilities/include/socket.hpp>
#include <utilities/include/threadpool.hpp>
#include <utilities/include/atend.hpp>

#include <thread>
#include <vector>
#include <memory>

using namespace std;
using namespace utilities;
using namespace server;

void worker(socket_stream, database&, volatile bool&);

void pp(unique_ptr<int>) {}

int main() {
	// Questo genera tutte le connesioni
	vector<database> db_connections(
			settings::inst().db_connection_number.value);

	//Solito thread pool
	thread_pool tPool;

	// Questo solo e unicamente per "eleganza"...
	on_return<>([&](){
		tPool.stop();
	});

	// Round robin per le db_connections!
	size_t rRobin = 0;

	// Ora inizializzo il socket
	socket_listener listener(2, 1, 6,
			inet_network(settings::inst().server_host.value.c_str()),
			settings::inst().server_port.value,
			settings::inst().queue_size.value);

	while(1) {
		tPool.execute(worker, listener.accept(), std::ref(db_connections[rRobin++]));
	}

	return -1; // Should never get here!
}

void worker(socket_stream sock, database& db, volatile bool&) {
	//STUB: invia "fatto" a tutti
	for(int i = 0; i < 8; i++)
		sock.send(true);
}
