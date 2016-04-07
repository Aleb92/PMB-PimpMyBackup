#include <database.hpp>

#include <utilities/include/exceptions.hpp>
#include <utilities/include/atend.hpp>

#include <cassert>
#include <vector>
#include <string>
#include <tuple>
#include <thread>

using namespace std;
using namespace server;
using namespace utilities;


template<typename T>
inline T db_column(sqlite3_stmt * stmt, int i);

template<>
inline int32_t db_column<int32_t>(sqlite3_stmt * stmt, int i) {
	return sqlite3_column_int(stmt, i);
}

template<>
inline uint32_t db_column<uint32_t>(sqlite3_stmt * stmt, int i) {
	return sqlite3_column_int(stmt, i);
}

template<>
inline int16_t db_column<int16_t>(sqlite3_stmt * stmt, int i) {
	return sqlite3_column_int(stmt, i);
}

template<>
inline uint16_t db_column<uint16_t>(sqlite3_stmt * stmt, int i) {
	return sqlite3_column_int(stmt, i);
}

template<>
inline int64_t db_column<int64_t>(sqlite3_stmt * stmt, int i) {
	return sqlite3_column_int64(stmt, i);
}

template<>
inline string db_column<string>(sqlite3_stmt * stmt, int i) {
	return string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
}

template<typename T>
inline void bind_one(sqlite3_stmt * stmt, T& val, int i);

//Implemento quelli che mi servono
template<>
inline void bind_one<uint32_t>(sqlite3_stmt * stmt, uint32_t& val, int i) {
	int v = sqlite3_bind_int(stmt, i, val);
	if (v != SQLITE_OK)
		throw v;
}

template<>
inline void bind_one<uint16_t>(sqlite3_stmt * stmt, uint16_t& val, int i) {
	int v = sqlite3_bind_int(stmt, i, val);
	if (v != SQLITE_OK)
		throw db_exception(v);
}

template<>
inline void bind_one<string>(sqlite3_stmt * stmt, string& val, int i) {
	int v = sqlite3_bind_text(stmt, i, val.c_str(), -1, nullptr);
	if (v != SQLITE_OK)
		throw db_exception(v);
}

template<>
inline void bind_one<int64_t>(sqlite3_stmt * stmt, int64_t& val, int i) {
	int v = sqlite3_bind_int64(stmt, val, i);
	if (v != SQLITE_OK)
		throw db_exception(v);
}

inline void bind_db(sqlite3_stmt*, int) {
}

template<typename A, typename ...T>
void bind_db(sqlite3_stmt*stmt, int i, A act, T ...args) {
	bind_one(stmt, act, i);
	bind_db(stmt, i + 1, args...);
}

user_context::user_context(std::string& user, std::string&pwd,
		std::string& file, database& _db) :
		usr(user), pass(pwd), path(file), db(_db) {
}

database::database(const char*db_name) {
	// Per prima cosa apro il database
	sqlite3*c;
	char*err;
	int r = sqlite3_open_v2(db_name, &c,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);
	//Inserisco subito il risultato in un oggetto che mi gestisca la memoria da solo
	connection = unique_ptr<sqlite3>(c);
	if (r != SQLITE_OK){
		if(c == nullptr)
			throw memory_exception();
		throw db_exception(r);
	}
	// Ok ora devo attivare i vincoli per le chiavi esterne
	if (sqlite3_exec(c, SQL_INIT, nullptr, nullptr, &err) != SQLITE_OK) {
		throw db_exception(err);
	}

	// BENE! Ora ho un database attivo! Devo generare tutte le prepared statement
	sqlite3_stmt * statement = nullptr;

	// auth
	r = sqlite3_prepare_v2(c, SQL_AUTH, sizeof(SQL_AUTH), &statement, nullptr);
	auth = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	// create
	r = sqlite3_prepare_v2(c, SQL_CREATE, sizeof(SQL_CREATE), &statement,
			nullptr);
	create = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	// chmod
	r = sqlite3_prepare_v2(c, SQL_CHMOD, sizeof(SQL_CHMOD), &statement,
			nullptr);
	chmod = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	//write
	r = sqlite3_prepare_v2(c, SQL_WRITE, sizeof(SQL_WRITE), &statement,
			nullptr);
	write = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	//move
	r = sqlite3_prepare_v2(c, SQL_MOVE, sizeof(SQL_MOVE), &statement, nullptr);
	move = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	//remove
	r = sqlite3_prepare_v2(c, SQL_DELETE, sizeof(SQL_DELETE), &statement,
			nullptr);
	remove = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	//version
	r = sqlite3_prepare_v2(c, SQL_VERSION, sizeof(SQL_VERSION), &statement,
			nullptr);
	version = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);

	//list
	r = sqlite3_prepare_v2(c, SQL_LIST_V, sizeof(SQL_LIST_V), &statement,
			nullptr);
	list = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(r);
}

user_context database::getUserContext(string&user, string&pass, string&path) {
	return user_context(user, pass, path, *this);
}

bool user_context::auth() {
	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.auth.get());
			sqlite3_clear_bindings(db.auth.get());
		});

	// Ora binding degli argomenti
	bind_db(db.auth.get(), 1, usr);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.auth.get())) {
		case SQLITE_DONE:
			return false;	// No such user!
		case SQLITE_ROW: {
			string pwd = db_column<string>(db.auth.get(), 0);
			return pwd == pass;
		}
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get());
		}
	}
}

void user_context::chmod(int64_t timestamp, uint32_t mod) {
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.chmod.get());
			sqlite3_clear_bindings(db.chmod.get());
		});

	// Ora binding degli argomenti
	bind_db(db.chmod.get(), 1, usr, path, timestamp, mod);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.chmod.get())) {
		case SQLITE_DONE:
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

void user_context::create(int64_t timestamp) {
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.create.get());
			sqlite3_clear_bindings(db.create.get());
		});

	// Ora binding degli argomenti
	bind_db(db.create.get(), 1, usr, path, timestamp);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.create.get())) {
		case SQLITE_DONE:
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

void user_context::move(int64_t timestamp, string& newPath) {
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.move.get());
			sqlite3_clear_bindings(db.move.get());
		});

	// Ora binding degli argomenti
	bind_db(db.move.get(), 1, usr, path, timestamp, newPath);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.move.get())) {
		case SQLITE_DONE:
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

void user_context::remove() {
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.remove.get());
			sqlite3_clear_bindings(db.remove.get());
		});

	// Ora binding degli argomenti
	bind_db(db.remove.get(), 1, usr, path);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.remove.get())) {
		case SQLITE_DONE:
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

void user_context::version(int64_t timestamp) {
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.version.get());
			sqlite3_clear_bindings(db.version.get());
		});

	// Ora binding degli argomenti
	bind_db(db.version.get(), 1, usr, path, timestamp);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.version.get())) {
		case SQLITE_DONE:
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

vector<int64_t> user_context::versions() {
	vector<int64_t> result;

	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.list.get());
			sqlite3_clear_bindings(db.list.get());
		});

	// Ora binding degli argomenti
	bind_db(db.list.get(), 1, usr, path);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.list.get())) {
		case SQLITE_DONE:
			return result;	// No such user!
		case SQLITE_ROW:
			result.push_back(db_column<int64_t>(db.list.get(), 0));
			break;
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

