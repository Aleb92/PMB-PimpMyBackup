#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <utilities/include/fsutil.hpp>

using namespace std;
using namespace utilities;

BOOST_AUTO_TEST_SUITE(utilities_test)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#include <windows.h>

BOOST_AUTO_TEST_CASE(MD5)
{
	unsigned char buff[MD5_DIGEST_LENGTH];

	wofstream testFile("test");
	testFile << "prova MD5";

	fileMD5(L"./test", buff);

	//TODO: windows

	testFile.close();
	remove("./test");
}
#else

BOOST_AUTO_TEST_CASE(MD5) {
	unsigned char buff[MD5_DIGEST_LENGTH];

	wofstream testFile("test");
	testFile << "prova MD5";

	fileMD5("./test", buff);

	//TODO: linux

	testFile.close();
	remove("./test");
}

#endif

BOOST_AUTO_TEST_SUITE_END()
