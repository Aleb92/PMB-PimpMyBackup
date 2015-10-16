#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Tests
#include <boost/test/unit_test.hpp>

#include <utilities/include/shared_queue.hpp>



using namespace utilities;
//using namespace std;

BOOST_AUTO_TEST_SUITE(shared_queue_test)


BOOST_AUTO_TEST_CASE(single_thread)
{
	BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE_END()
