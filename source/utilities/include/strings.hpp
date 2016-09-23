#ifndef SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_
#define SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_

#include <cwchar>
#include <stddef.h>
#include <vector>
#include <string>
#include <ostream>

namespace utilities{

std::string utf8_encode(const std::wstring&);
std::wstring utf8_decode(const std::string&);

const wchar_t* wcsrchr (const wchar_t*, wchar_t, size_t);

/**
 * Splitta la stringa in tante parti tutte le volte che trova un'occorrenza del carattere separatore
 * @param Stringa da dividere
 * @param Carattere separatore
 * @return Vector di stringhe contenente le divisioni
 */
std::vector<std::string> split(const std::string &, char);

/**
 * Come la split normale ma agisce su un vector che gia potrebbe contenere delle stringhe e aggiunge
 * @param Stringa da dividere
 * @param Carattere separatore
 * @param Vector a cui aggiungere le nuove suddivioni
 * @return Lo stesso vector (viene ritornato per comodità solo per poter usare split."metodo del vettore riempito")
 */
std::vector<std::string> &split(const std::string &, char,
		std::vector<std::string> &);

/**
 * Splitta la stringa in tante parti tutte le volte che trova un'occorrenza del carattere separatore (WCHAR)
 * @param Stringa da dividere
 * @param Carattere separatore
 * @return Vector di stringhe contenente le divisioni
 */
std::vector<std::wstring> split(const std::wstring &, wchar_t);

/**
 * Come la split normale ma agisce su un vector che gia potrebbe contenere delle stringhe e aggiunge (WCHAR)
 * @param Stringa da dividere
 * @param Carattere separatore
 * @param Vector a cui aggiungere le nuove suddivioni
 * @return Lo stesso vector (viene ritornato per comodità solo per poter usare split."metodo del vettore riempito")
 */
std::vector<std::wstring> &split(const std::wstring &, wchar_t,
		std::vector<std::wstring> &);

/**
 * Questo metodo divide la stringa come la split ma solo una volta quindi ritorna un pair (WCHAR)
 * @param Stringa da dividere
 * @param Carattere separatore
 * @return Coppia di stringhe splittate
 */
std::pair<std::wstring, std::wstring> splitOnce(const std::wstring &, wchar_t);

/**
 * Questo metodo divide la stringa come la split ma solo una volta quindi ritorna un pair
 * @param Stringa da dividere
 * @param Carattere separatore
 * @return Coppia di stringhe splittate
 */
std::pair<std::string, std::stringstream> splitOnce(const std::string &, char);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
template<typename _C>
inline std::basic_ostream<_C>& operator<< (std::basic_ostream<_C>& out, const FILETIME time) {
	ULARGE_INTEGER large;
	large.HighPart = time.dwHighDateTime;
	large.LowPart = time.dwLowDateTime;
	return out << large.QuadPart;
}
#endif

inline std::ostream& operator<<(std::ostream& l, std::wstring str) {
	return l << utilities::utf8_encode(str);
}

#endif /* SOURCE_UTILITIES_INCLUDE_STRINGS_HPP_ */
