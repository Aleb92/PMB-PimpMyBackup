#include <database.hpp>

#include <utilities/include/debug.hpp>
#include <utilities/include/exceptions.hpp>
#include <utilities/include/atend.hpp>

#include <cassert>
#include <vector>
#include <string>
#include <tuple>
#include <thread>
#include <cstring>

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
	LOGF;
	const char * c = (const char*)(sqlite3_column_text(stmt, i));
	LOGD((unsigned long long)c);
	LOGD(c);
	return string(c);
}

template<typename T>
inline void bind_one(sqlite3_stmt * stmt, T val, int i);

//Implemento quelli che mi servono
template<>
inline void bind_one<uint32_t>(sqlite3_stmt * stmt, uint32_t val, int i) {
	LOGF;
	LOGD(val);

	int v = sqlite3_bind_int(stmt, i, val);
	if (v != SQLITE_OK)
		throw v;
}

template<>
inline void bind_one<uint16_t>(sqlite3_stmt * stmt, uint16_t val, int i) {
	LOGF;
	LOGD(val);
	int v = sqlite3_bind_int(stmt, i, val);
	if (v != SQLITE_OK)
		throw db_exception(v,__LINE__, __func__, __FILE__);
}

template<>
inline void bind_one<const char*>(sqlite3_stmt * stmt, const char* val, int i) {
	LOGF;
	LOGD(val << " : " << strlen(val) << " : " << i);
	int v = sqlite3_bind_text(stmt, i, val, -1, nullptr);
	if (v != SQLITE_OK)
		throw db_exception(v,__LINE__, __func__, __FILE__);
}

template<>
inline void bind_one<int64_t>(sqlite3_stmt * stmt, int64_t val, int i) {
	LOGF;
	LOGD(val);
	int v = sqlite3_bind_int64(stmt, i, val);
	if (v != SQLITE_OK)
		throw db_exception(v,__LINE__, __func__, __FILE__);
}

inline void bind_db(sqlite3_stmt*, int) {
	LOGF;
}

template<typename A, typename ...T>
void bind_db(sqlite3_stmt*stmt, int i, A act, T ...args) {
	LOGF;
	bind_one(stmt, act, i);
	bind_db(stmt, i + 1, args...);
}

user_context::user_context(std::string& user, std::string&pwd,
		std::string& file, database& _db) :
		usr(user), pass(pwd), path(file), db(_db) {
	LOGF;
}

database::database(const char*db_name) {
	LOGF;
	// Per prima cosa apro il database
	sqlite3*c;
	char*err;
	int r = sqlite3_open_v2(db_name, &c,
	SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, nullptr);
	//Inserisco subito il risultato in un oggetto che mi gestisca la memoria da solo
	connection = unique_ptr<sqlite3>(c);
	if (r != SQLITE_OK) {
		if (c == nullptr)
			throw memory_exception(__LINE__, __func__, __FILE__);
		throw db_exception(r,__LINE__, __func__, __FILE__);
	}
	// Ok ora devo attivare i vincoli per le chiavi esterne
	if (sqlite3_exec(c, SQL_INIT, nullptr, nullptr, &err) != SQLITE_OK) {
		throw db_exception(err,__LINE__, __func__, __FILE__);
	}

	// BENE! Ora ho un database attivo! Devo generare tutte le prepared statement
	sqlite3_stmt * statement = nullptr;

	// auth
	r = sqlite3_prepare_v2(c, SQL_AUTH, sizeof(SQL_AUTH), &statement, nullptr);
	auth = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	// create
	r = sqlite3_prepare_v2(c, SQL_CREATE, sizeof(SQL_CREATE), &statement,
			nullptr);
	create = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	// chmod
	r = sqlite3_prepare_v2(c, SQL_CHMOD, sizeof(SQL_CHMOD), &statement,
			nullptr);
	chmod = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//write
	r = sqlite3_prepare_v2(c, SQL_WRITE, sizeof(SQL_WRITE), &statement,
			nullptr);
	write = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//move
	r = sqlite3_prepare_v2(c, SQL_MOVE, sizeof(SQL_MOVE), &statement, nullptr);
	move = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//apply
	r = sqlite3_prepare_v2(c, SQL_APPLY, sizeof(SQL_APPLY), &statement, nullptr);
	apply = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//remove
	r = sqlite3_prepare_v2(c, SQL_DELETE, sizeof(SQL_DELETE), &statement,
			nullptr);
	remove = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//version
	r = sqlite3_prepare_v2(c, SQL_VERSION, sizeof(SQL_VERSION), &statement,
			nullptr);
	version = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//list
	r = sqlite3_prepare_v2(c, SQL_LIST_V, sizeof(SQL_LIST_V), &statement,
			nullptr);
	list = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//sync
	r = sqlite3_prepare_v2(c, SQL_SYNC, sizeof(SQL_SYNC), &statement, nullptr);
	sync = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	//exists
	r = sqlite3_prepare_v2(c, SQL_EXISTS, sizeof(SQL_EXISTS), &statement, nullptr);
	version_exists = unique_ptr<sqlite3_stmt>(statement);
	if (r != SQLITE_OK)
		throw db_exception(c,__LINE__, __func__, __FILE__);

	// DEBUG
	sqlite3_trace(connection.get(), utilities::debug::db_trace, nullptr);
}

user_context database::getUserContext(string&user, string&pass, string&path) {
	LOGF;
	return user_context(user, pass, path, *this);
}

bool user_context::auth() {
	LOGF;
	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.auth.get());
			sqlite3_clear_bindings(db.auth.get());
		});

	// Ora binding degli argomenti
	bind_db(db.auth.get(), 1, usr.c_str());

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.auth.get())) {
		case SQLITE_DONE:
			LOGD("utente non trovato");
			return false;	// No such user!
		case SQLITE_ROW: {
			string pwd = db_column<string>(db.auth.get(), 0);
			LOGD("DB PWD:" << pwd << pwd.length());
			LOGD("NET PWD" << pass << pass.length());
			return pwd == pass;
		}
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

bool user_context::version_exists(string& id) {
	LOGF;
	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.version_exists.get());
			sqlite3_clear_bindings(db.version_exists.get());
		});

	// Ora binding degli argomenti
	bind_db(db.version_exists.get(), 1, usr.c_str(), path.c_str());

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.version_exists.get())) {
		case SQLITE_DONE:
			LOGD("No version found");
			return false;	// No such user!
		case SQLITE_ROW: 
			{
				string vID = db_column<string>(db.version_exists.get(), 0);
				LOGD("DB VERSION: " << vID);
				LOGD("NEW VERSION: " << id);
				if(vID == id)
					return true;
			}
			break;
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

void user_context::chmod(int64_t timestamp, uint32_t mod) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.chmod.get());
			sqlite3_clear_bindings(db.chmod.get());
		});

	// Ora binding degli argomenti
	bind_db(db.chmod.get(), 1, usr.c_str(), path.c_str(), timestamp, mod);

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
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

void user_context::create(int64_t timestamp) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.create.get());
			sqlite3_clear_bindings(db.create.get());
		});

	string file_id = std::to_string(timestamp);
	
	// Ora binding degli argomenti
	bind_db(db.create.get(), 1, usr.c_str(), path.c_str(), timestamp, file_id.c_str());

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
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

void user_context::move(int64_t timestamp, string& newPath) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.move.get());
			sqlite3_clear_bindings(db.move.get());
		});

	// Ora binding degli argomenti
	bind_db(db.move.get(), 1, usr.c_str(), path.c_str(), timestamp, newPath.c_str());

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.move.get())) {
		case SQLITE_DONE:
			return;
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

void user_context::apply(int64_t timestamp) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.apply.get());
			sqlite3_clear_bindings(db.apply.get());
		});

	// Ora binding degli argomenti
	bind_db(db.apply.get(), 1, usr.c_str(), timestamp);

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.apply.get())) {
		case SQLITE_DONE:
			return;
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}


void user_context::remove(int64_t timestamp) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.remove.get());
			sqlite3_clear_bindings(db.remove.get());
		});

	// Ora binding degli argomenti
	bind_db(db.remove.get(), 1, usr.c_str(), path.c_str(), timestamp);

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
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

string user_context::version(int64_t timestamp) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.version.get());
			sqlite3_clear_bindings(db.version.get());
		});

	// Ora binding degli argomenti
	bind_db(db.version.get(), 1, usr.c_str(), path.c_str(), timestamp);

	bool set = false;
	string fileID;

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.version.get())) {
		case SQLITE_ROW:
			fileID = db_column<string>(db.version.get(), 0);
			set = true;
			break;
		case SQLITE_DONE:
			if(set)
				return fileID;	// FATTO!
			else
				throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

vector<int64_t> user_context::versions() {
	LOGF;
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
	bind_db(db.list.get(), 1, usr.c_str(), path.c_str());

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
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

vector<pair<string, int64_t>> user_context::sync() {
	LOGF;
	vector<pair<string, int64_t>> result;

	//Prima cosa: lock! Dalle specifiche sqlite solo un thread alla volta può
	//usare la connessione.
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.sync.get());
			sqlite3_clear_bindings(db.sync.get());
		});

	// Ora binding degli argomenti
	bind_db(db.sync.get(), 1, usr.c_str());

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.sync.get())) {
		case SQLITE_DONE:
			return result;	// No such user!
		case SQLITE_ROW:
			result.push_back(make_pair(db_column<string>(db.sync.get(), 0), db_column<int64_t>(db.sync.get(), 1)));
			break;
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}
}

void user_context::write(int64_t time, string& fileID) {
	LOGF;
	lock_guard<mutex> guard(db.busy);

	on_return<> ret([&] {
		// On return resetto statement e bindings
			sqlite3_reset(db.write.get());
			sqlite3_clear_bindings(db.write.get());
		});

	// Ora binding degli argomenti
	bind_db(db.write.get(), 1, usr.c_str(), path.c_str(), time, fileID.c_str());

	// Quindi eseguo
	while (1) {
		switch (sqlite3_step(db.write.get())) {
		case SQLITE_DONE:
			//if(sqlite3_changes(db.connection.get()) == 0)
			//	throw db_exception("No such file",__LINE__, __func__, __FILE__);
			return;	// FATTO!
		case SQLITE_BUSY:
			// Qui non ci dovrebbe mai arrivare(WAL mode)...
			// Comunque nel caso, prima lascio fare qualcosa agli altri
			this_thread::yield();
			// Poi riprovo.
			break;
		default:
			throw db_exception(db.connection.get(),__LINE__, __func__, __FILE__);
		}
	}

}

