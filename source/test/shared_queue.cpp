#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <utilities/include/shared_queue.hpp>

#include <thread>
#include <cstdlib>
#include <ctime>




using namespace utilities;
using namespace std;

struct fix {
	fix() {
		srand(time(NULL));
	}
};

BOOST_FIXTURE_TEST_SUITE(shared_queue_test, fix)

BOOST_AUTO_TEST_CASE(singleth_singleq)
{
	int num = rand() % 1000;
	int vett[1000];
	for(int i = 0; i < num; i++) {
		vett[i] = rand();
		shared_queue<int>::inst().enqueue(vett[i]);
	}

	for(int i = 0; i < num; i++)
		BOOST_CHECK_EQUAL(vett[i], shared_queue<int>::inst().dequeue());
}

BOOST_AUTO_TEST_CASE(singleth_doubleq){

	int num = rand() % 1000;
	int vett[1000];
	double dvett[1000];
	for(int i = 0; i < num; i++) {
		vett[i] = rand();
		dvett[i] = rand()*1.0f;
		shared_queue<int>::inst().enqueue(vett[i]);
		shared_queue<double>::inst().enqueue(dvett[i]);
	}

	for(int i = 0; i < num; i++) {
		BOOST_CHECK_EQUAL(vett[i], shared_queue<int>::inst().dequeue());
		BOOST_CHECK_EQUAL(dvett[i], shared_queue<double>::inst().dequeue());
	}
}

class cbk {
	cbk() : count(0) { }
public:
	int count;


	void push(int& i) {
		++count;
	}

	void pop(int&i) {
		--count;
	}

	static cbk c;
};
cbk cbk::c;

BOOST_AUTO_TEST_CASE(callbacks) {
	int num = rand() % 1000;
	int vett[1000];

	typedef shared_queue<int, cbk, &cbk::c, &cbk::push, &cbk::pop> sq;

	for(int i = 0; i < num; i++) {
		vett[i] = rand();
		sq::inst().enqueue(vett[i]);
	}
	BOOST_CHECK(cbk::c.count == num);
	for(int i = 0; i < num; i++)
		BOOST_CHECK_EQUAL(vett[i], sq::inst().dequeue());
	BOOST_CHECK(cbk::c.count == 0);
}



BOOST_AUTO_TEST_CASE(doubleth_singleq)
{
	int num = rand() % 1000;
	int vett[1000];
	thread th( [num, &vett]() {
		for(int i = 0; i < num; i++) {
			vett[i] = rand();
			shared_queue<int>::inst().enqueue(vett[i]);
		}
	});

	for(int i = 0; i < num; i++)
		BOOST_CHECK_EQUAL(vett[i], shared_queue<int>::inst().dequeue());

	if(th.joinable()) th.join();

}



BOOST_AUTO_TEST_SUITE_END()
