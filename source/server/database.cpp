#include <database.hpp>

#include <cassert>
#include <vector>
#include <string>
#include <tuple>
#include <thread>

using namespace std;
using namespace server;

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
inline void bind_one<string>(sqlite3_stmt * stmt, string& val, int i) {
	int v = sqlite3_bind_text(stmt, i, val.c_str(), -1, nullptr);
	if (v != SQLITE_OK)
		throw v;
}

template<>
inline void bind_one<int64_t>(sqlite3_stmt * stmt, int64_t& val, int i) {
	int v = sqlite3_bind_int64(stmt, val, i);
	if (v != SQLITE_OK)
		throw v;
}

void bind(sqlite3_stmt*) { }

template<typename A, typename ...T>
void bind(sqlite3_stmt*stmt, int i, A act, T...args){
	bind_one(stmt, act, i);
	bind(stmt, i+1, args...);
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
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	// Ok ora devo attivare i vincoli per le chiavi esterne
	if (sqlite3_exec(c, SQL_INIT, nullptr, nullptr, &err) != SQLITE_OK) {
		string s(err);
		sqlite3_free(err);
		throw s;
	}

	// BENE! Ora ho un database attivo! Devo generare tutte le prepared statement
	sqlite3_stmt * statement = nullptr;

	// auth
	r = sqlite3_prepare_v2(c, SQL_AUTH, sizeof(SQL_AUTH), &statement, nullptr);
	auth = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	// create
	r = sqlite3_prepare_v2(c, SQL_CREATE, sizeof(SQL_CREATE), &statement,
			nullptr);
	create = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	// chmod
	r = sqlite3_prepare_v2(c, SQL_CHMOD, sizeof(SQL_CHMOD), &statement,
			nullptr);
	chmod = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	//write
	r = sqlite3_prepare_v2(c, SQL_WRITE, sizeof(SQL_WRITE), &statement,
			nullptr);
	write = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	//move
	r = sqlite3_prepare_v2(c, SQL_MOVE, sizeof(SQL_MOVE), &statement, nullptr);
	move = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	//remove
	r = sqlite3_prepare_v2(c, SQL_DELETE, sizeof(SQL_DELETE), &statement,
			nullptr);
	remove = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	//version
	r = sqlite3_prepare_v2(c, SQL_VERSION, sizeof(SQL_VERSION), &statement,
			nullptr);
	version = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);

	//list
	r = sqlite3_prepare_v2(c, SQL_LIST_V, sizeof(SQL_LIST_V), &statement,
			nullptr);
	list = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw sqlite3_errmsg(c);
}

user_context database::getUserContext(string&user, string&pass, string&path) {
	return user_context(user, pass, path, *this);
}

bool user_context::auth() {
	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	// Ora binding degli argomenti
	bind(db.auth.get(), 0, usr, pass);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.auth.get())) {
		case SQLITE_DONE:
			return false;	// No such user!
		case SQLITE_ROW:
			// TODO
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			continue;
		default:
			throw sqlite3_errmsg(db.connection.get());
		}
	}
}

//TODO
void user_context::chmod(int64_t timestamp, uint16_t mod) {
}

void user_context::create(int64_t timestamp) {
}

void user_context::move(int64_t timestamp, string& str) {
}

void user_context::remove() {
}

void user_context::version(int64_t timestamp) {
}

vector<string> user_context::versions() {
	return vector<string>();
}

