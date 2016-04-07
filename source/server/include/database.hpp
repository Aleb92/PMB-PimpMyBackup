#ifndef SOURCE_SERVER_INCLUDE_DATABASE_HPP_
#define SOURCE_SERVER_INCLUDE_DATABASE_HPP_

#include <settings.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <sqlite3.h>



namespace std {
template<>
struct default_delete<sqlite3> {
	constexpr default_delete() = default;
	inline void operator()(sqlite3*p) const {
		sqlite3_close(p);
	}
};

template<>
struct default_delete<sqlite3_stmt> {
	constexpr default_delete() = default;
	inline void operator()(sqlite3_stmt*p) const {
		sqlite3_finalize(p);
	}
};
}

namespace server {

#define SQL_INIT "PRAGMA foreign_keys = ON;PRAGMA journal_mode=WAL;"
#define SQL_AUTH "SELECT password WHERE username=?1"
#define SQL_CREATE "INSERT INTO files (username, path, time_stamp) VALUES(?1,?2,?3)"
#define SQL_CHMOD "UPDATE files SET time_stamp=?3, mod=?4 WHERE username=?1 AND path=?2"
#define SQL_WRITE "UPDATE files SET time_stamp=?3, file_id=?4 WHERE username=?1 AND path=?2"
#define SQL_MOVE "UPDATE files SET time_stamp=?3, path=?4 WHERE username=?1 AND path=?2"
#define SQL_DELETE "DELETE FROM files WHERE username=?1 AND path=?2"
#define SQL_VERSION "WITH tmp AS (SELECT time_stamp, mod, file_id FROM history WHERE WHERE username=?1 AND path=?2 AND time_stamp=?3) UPDATE files SET time_stamp=(SELECT time_stamp FROM tmp),mod=(SELECT mod FROM tmp),file_id=(SELECT file_id FROM tmp) WHERE username=?1 AND path=?2"
#define SQL_LIST_V "SELECT time_stamp FROM history WHERE username=?1 AND path=?2 ORDER BY time_stamp DESC"

class database;

class user_context {
	std::string& usr, &pass, &path;
	database& db;

	friend class database;
	user_context(std::string&, std::string&, std::string&, database&);
public:
	bool auth();
	void create(int64_t);
	void chmod(int64_t, uint16_t);
	std::string write(int64_t);
	void move(int64_t, std::string&);
	void remove();
	std::string version(int64_t);
	std::vector<int64_t> versions();
};


class database {
	std::unique_ptr<sqlite3> connection;
	std::unique_ptr<sqlite3_stmt> auth, create,
			chmod, write, move, remove, version, list;
	std::mutex busy;
	friend class user_context;
public:
	database(const char*db_name = settings::inst().db_name.value.c_str());
	user_context getUserContext(std::string&, std::string&, std::string&);
};

}

#endif /* SOURCE_SERVER_INCLUDE_DATABASE_HPP_ */
