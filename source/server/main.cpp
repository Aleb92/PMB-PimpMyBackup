#include <settings.hpp>
#include <database.hpp>
#include <protocol.hpp>

#include <utilities/include/socket.hpp>
#include <utilities/include/threadpool.hpp>
#include <utilities/include/atend.hpp>

#include <thread>
#include <vector>
#include <memory>
#include <utility>

using namespace std;
using namespace utilities;
using namespace server;

void worker(socket_stream, database&, volatile bool&);

int main() {
	// Questo genera tutte le connesioni
	vector<database> db_connections(
			settings::inst().db_connection_number.value);

	//Solito thread pool
	thread_pool tPool;

	// Questo solo e unicamente per "eleganza"...
	on_return<>([&]() {
		tPool.stop();
	});

	// Round robin per le db_connections!
	size_t rRobin = 0;

	// Ora inizializzo il socket
	socket_listener listener(AF_INET, SOCK_STREAM, IPPROTO_TCP,
			inet_network(settings::inst().server_host.value.c_str()),
			settings::inst().server_port.value,
			settings::inst().queue_size.value);

	while (1) {
		tPool.execute(worker, listener.accept(),
				std::ref(db_connections[rRobin++]));
		if (rRobin == db_connections.size())
			rRobin = 0;
	}

	return -1; // Should never get here!
}

void move(socket_stream& sock, user_context& context, int64_t ts){
	string mv = sock.recv<string>();
	context.move(ts, mv);
}

void create(socket_stream& sock, user_context& context, int64_t ts) {
	context.create(ts);
}

void remove(socket_stream& sock, user_context& context, int64_t){
	context.remove();
}

void chmodFile(socket_stream& sock, user_context& context, int64_t ts){
	context.chmod(ts, sock.recv<int32_t>());
}

void moveDir(socket_stream&, user_context&, int64_t) {

}

void writeFile(socket_stream&, user_context&, int64_t) {

}

void list(socket_stream&, user_context&, int64_t) {
}

void version(socket_stream& sock, user_context& context, int64_t ts) {
	char buffer[BUFF_LENGHT] = { 0 };

	string fileID = context.usr + "/" + context.version(ts);

	FILE* file = fopen(fileID.c_str(), "rb");
	if (file == NULL)
		throw fs_exception();

	on_return<> ret([file]() {
		fclose(file);
	});

	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	sock.send(size);

	while (feof(file)) {
		socket_base::SOCK_STATE state = sock.getState();
		if (state & socket_base::READ_READY) {
			if (sock.recv<bool>())
				return;
		}
		size_t readn = fread(buffer, BUFF_LENGHT, 1, file);
		sock.send(buffer, readn);
	}
}

void worker(socket_stream sock, database& db, volatile bool&) {

	const pair<opcode, void (*)(socket_stream&, user_context&, int64_t)> flag[] =
			{ { MOVE, move }, { CREATE, create }, { REMOVE, remove }, { CHMOD,
					chmodFile }, { MOVE_DIR, moveDir }, { VERSION, version }, {
					LIST, }, { WRITE, writeFile } };

	string username = sock.recv<string>();
	string password = sock.recv<string>();
	string fileName = sock.recv<string>();

	auto context = db.getUserContext(username, password, fileName);
	if (!context.auth()) {
		sock.send<bool>(false);
		return;
	}

	opcode opCode = sock.recv<opcode>();
	uint64_t timestamp[8];

	for (auto& ts : timestamp)
		ts = sock.recv<uint64_t>();

	for (int i = 0; i < 8; i++) {
		if (opCode & flag[i].first)
			(flag[i].second)(sock, context, timestamp[i]);
	}

}
