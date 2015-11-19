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

	namespace impl {
		struct nihil {};
	}

	template<typename T, typename L = impl::nihil, L* _l = nullptr, void (L::*push)(T&) = nullptr, void (L::*pop)(T&) = nullptr >
	class shared_queue : public singleton<shared_queue<T, L, _l, push, pop>> {
		std::deque<T> data;
		std::mutex lk;
		std::condition_variable cv;

		friend class singleton<shared_queue<T, L, _l, push, pop>>;

		shared_queue() = default;
	public:

		void enqueue(T& obj) {
			std::lock_guard<std::mutex> guard(lk);
			data.push_back(obj);
			if(_l && push) (_l->*push)(obj);
			cv.notify_all();
		}

		T dequeue(void){
			std::unique_lock<std::mutex> guard(lk);

			on_return<> ret([this](){
				if(_l && push) (_l->*pop)(data.front());
				data.pop_front();
			});

			cv.wait(guard, [this](){
				return !data.empty();
			});
			return data.front();
		}

		inline bool empty() const {
			return data.empty();
		}

	};
}


#endif /* SOURCE_UTILITIES_INCLUDE_SHARED_QUEUE_HPP_ */
