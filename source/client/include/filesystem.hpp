/*
 * filesystem.hpp
 *
 *  Created on: 10 nov 2015
 *      Author: Marco
 */

#ifndef SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_
#define SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_

#include <server/include/protocol.hpp>

#include <forward_list>
#include <unordered_map>
#include <string>
#include <Windows.h>
#include <openssl/md5.h>

namespace client {

enum file_mod : uint16_t {
	ROOT_EXEC = 1<<0,
	ROOT_WRITE = 1 <<1,
	ROOT_READ = 1<<2,
	ALL_EXEC = 1<<3,
	ALL_WRITE = 1<<4,
	ALL_READ = 1<<5,
	USER_EXEC = 1<<6,
	USER_WRITE = 1<<7,
	USER_READ = 1<<8
};

inline file_mod operator|(file_mod lh, file_mod rh){ return static_cast<file_mod>(lh | rh); }
inline file_mod operator&(file_mod lh, file_mod rh){ return static_cast<file_mod>(lh & rh); }
inline file_mod operator^(file_mod lh, file_mod rh){ return static_cast<file_mod>(lh ^ rh); }
inline file_mod operator~(file_mod lh){ return static_cast<file_mod>(~lh); }

class filesystem;

struct file_info {
	FILETIME lastModified;
	client::file_mod mod;
	unsigned char checksum[MD5_DIGEST_LENGTH];
};

class directory {

	using file_table = std::unordered_map<std::wstring, client::file_info>;
	using dir_list = std::forward_list<directory*>;

	friend class filesystem;

	void move(filesystem& fsys, directory& from);

	file_table files;
	dir_list dirList;

	static const std::wstring null_dir;

	const std::wstring*name = &null_dir;
public:

	typedef dir_list::iterator dir_iterator;
	typedef file_table::iterator file_iterator;

	typedef dir_list::const_iterator const_dir_iterator;
	typedef file_table::const_iterator const_file_iterator;

	//TODO: in seguito sarebbe carino implementare il move costructor
	//		per evitare una copia delle liste/tabelle.

	directory() = default;

	inline dir_iterator dbegin(){
		return dirList.begin();
	}
	inline dir_iterator dend(){
		return dirList.end();
	}

	inline const_dir_iterator dbegin() const{
		return dirList.begin();
	}
	inline const_dir_iterator dend() const{
		return dirList.end();
	}

	inline file_iterator fbegin(){
		return files.begin();
	}
	inline file_iterator fend(){
		return files.end();
	}

	inline const_file_iterator fbegin() const{
		return files.begin();
	}
	inline const_file_iterator fend() const{
		return files.end();
	}

	inline const std::wstring& get_name() const { return *name; }

	file_info& get(const std::wstring&);
	file_info remove(const std::wstring&);
	file_info& add(const std::wstring&);
};

class filesystem {

	using dir_table = std::unordered_map<std::wstring, directory>;
	dir_table directories;
	bool fromFile;///< per uso eventuale
	void loadFS(const std::wstring&,const std::wstring&);
public:

	filesystem();

	~filesystem();


	const directory& root();

	file_info& get_file(const wchar_t*, size_t s = 0);
	directory& get_dir(const wchar_t*, size_t s = 0);

	void delete_dir(const wchar_t*, size_t s = 0);
	void delete_file(const wchar_t*, size_t s = 0);
	void move_dir(const wchar_t*, const wchar_t*);
	void move_file(const wchar_t*, size_t, const wchar_t*, size_t);



	/**
	 *
	 * @param
	 * @param s
	 * @return
	 * @remarks Il file aggiunto va riempito!!!!
	 */
	file_info& new_file(const wchar_t*, size_t s = 0);
	directory& new_dir(const wchar_t*, size_t s = 0);
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_ */
