
/*
 * queue.hpp
 *
 *  Created on: 14 ott 2015
 *      Author: Marco
 */

#ifndef SOURCE_UTILITIES_INCLUDE_SHARED_QUEUE_HPP_
#define SOURCE_UTILITIES_INCLUDE_SHARED_QUEUE_HPP_

#include <deque>
#include <condition_variable>
#include <mutex>
#include <utilities/include/atend.hpp>
#include <utilities/include/singleton.hpp>

namespace utilities {

	template<typename T>
	class shared_queue : public singleton<shared_queue<T>> {
		std::deque<T> data;
		std::mutex lk;
		std::condition_variable cv;

		friend class singleton<shared_queue<T>>;

		shared_queue(){};
	public:

		void enqueue(const T obj) {
			std::lock_guard<std::mutex> guard(lk);
			data.push_back(obj);

			cv.notify_all();
		}

		T dequeue(void){
			std::unique_lock<std::mutex> guard(lk);

			on_return<> ret([this](){

				data.pop_front();
			});//FIXME: viene davvero poi ottimizzato?

			cv.wait(guard, [this](){
				return !data.empty();
			});
			return data.front();
		}

		inline bool empty() {
			//FIXME: Serve sincronizzare?
			std::lock_guard<std::mutex> guard(lk);
			return data.empty();
		}

	};

}


#endif /* SOURCE_UTILITIES_INCLUDE_SHARED_QUEUE_HPP_ */
