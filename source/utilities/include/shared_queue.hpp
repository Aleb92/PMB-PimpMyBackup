
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


		struct nihil : public singleton<nihil> {
			//FIXME: Non mi piace! E' semplicemente orrido.
			friend class singleton<nihil>;
		};
	}

	template<typename T, typename L = impl::nihil,
			void (L::*push)(const T&) = nullptr, void (L::*pop)(const T&) = nullptr >
	class shared_queue : public singleton<shared_queue<T, L, push, pop>> {
		std::deque<T> data;
		std::mutex lk;
		std::condition_variable cv;
		L& _l;

		friend class singleton<shared_queue<T, L, push, pop>>;

		//FIXME: qui suppongo che L sia un singleton!
		shared_queue() : _l(L::inst()) {};
	public:

		void enqueue(const T obj) {
			std::lock_guard<std::mutex> guard(lk);
			data.push_back(obj);
			if(push) (_l.*push)(obj);
			cv.notify_all();
		}

		T dequeue(void){
			std::unique_lock<std::mutex> guard(lk);

			on_return<> ret([this](){
				if(pop) (_l.*pop)(data.front());
				data.pop_front();
			});//FIXME: viene davvero poi ottimizzato?

			cv.wait(guard, [this](){//FIXME: e la distruzione?
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
