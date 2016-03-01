#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <utilities/include/fsutil.hpp>

#include <openssl/md5.h>
#include <cstdio>
#include <cstring>
#include <random>
#include <ostream>
#include <fstream>

using namespace std;
using namespace utilities;

#define MD5_TEST_FILENAME "md5_test"
#define BIG_BUFF_SIZE 10000

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
# define MD5_TEST_FILENAME_W L"md5_test"
#else
# define MD5_TEST_FILENAME_W MD5_TEST_FILENAME
#endif


BOOST_AUTO_TEST_SUITE(utilities_test)


#include <windows.h>

/**
 * Test che prova a generare un MD5 per un file contenente una semplice stringa
 */
BOOST_AUTO_TEST_CASE(MD5_static)
{
	unsigned char buff[MD5_DIGEST_LENGTH] = { 0 }, check[MD5_DIGEST_LENGTH] = { 0 };
	const unsigned char static_try[] = "prova MD5";

	ofstream testFile(MD5_TEST_FILENAME);
	testFile << static_try;
	testFile.close(); // Questo fa si che venga scritto

	fileMD5(MD5_TEST_FILENAME_W, buff);
	MD5(static_try, sizeof(static_try)-1, check);

	BOOST_CHECK(memcmp(buff, check, MD5_DIGEST_LENGTH) == 0);
	remove(MD5_TEST_FILENAME);
}

/**
 * Questo test genera un buffer binario casuale, ne calcola l'MD5 sia tramite
 * file che tramite memoria e li confronta.
 */
BOOST_AUTO_TEST_CASE(MD5_random) {
	unsigned char buff[MD5_DIGEST_LENGTH], check[MD5_DIGEST_LENGTH];
	unsigned char bigBuff[BIG_BUFF_SIZE];

	random_device rd;
	// Ora genero a caso dei numeri per riempire
	for(int i = 0; i < BIG_BUFF_SIZE; i++)
		bigBuff[i] = rd();

	ofstream testFile(MD5_TEST_FILENAME, ios::binary | ios::out);
	testFile.write(reinterpret_cast<char*>(bigBuff), BIG_BUFF_SIZE);
	testFile.close();

	fileMD5(MD5_TEST_FILENAME_W, buff);
	MD5(bigBuff, BIG_BUFF_SIZE, check);

	BOOST_CHECK(memcmp(buff, check, MD5_DIGEST_LENGTH) == 0);
	remove(MD5_TEST_FILENAME);
}

BOOST_AUTO_TEST_SUITE_END()
