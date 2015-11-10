/*
 * filesystem.hpp
 *
 *  Created on: 10 nov 2015
 *      Author: Marco
 */

#ifndef SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_
#define SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_

#include <server/include/protocol.hpp>

#include <list>
#include <map>
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



struct file_info {
	FILETIME lastModified;
	file_mod mod;
	char checksum[MD5_DIGEST_LENGTH];
};

class directory {

	using file_table = std::unordered_map<std::wstring, file_info>;
	using dir_list = std::list<directory&>;

	friend class filesystem;

	file_table files;
	dir_list list;

public:

	typedef dir_list::iterator dir_iterator;
	typedef file_table::iterator file_iterator;

	typedef dir_list::const_iterator const_dir_iterator;
	typedef file_table::const_iterator const_file_iterator;

	dir_iterator dbegin();
	dir_iterator dend();

	const_dir_iterator dbegin() const;
	const_dir_iterator dend() const;

	file_iterator fbegin();
	file_iterator fend();

	const_file_iterator fbegin() const;
	const_file_iterator fend() const;

	file_info& get(const std::wstring&);
	file_info remove(const std::wstring&);
	void add(const std::wstring&);

};

class filesystem {

	using dir_table = std::unordered_map<std::wstring, directory>;
	dir_table directories;
public:
	filesystem(HANDLE);
	virtual ~filesystem();

	file_info& get_file(const wchar_t*);
	directory& get_dir(const wchar_t*);

	void delete_dir(const wchar_t*);
	void delete_file(const wchar_t*);
	void move_dir(const wchar_t*, const wchar_t*);
	void move_file(const wchar_t*, const wchar_t*);
	void new_file(const wchar_t*);
	void new_dir(const wchar_t*);
};

} /* namespace client */

#endif /* SOURCE_CLIENT_INCLUDE_FILESYSTEM_HPP_ */
