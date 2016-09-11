
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
#include <utilities/include/debug.hpp>
#include <utilities/include/strings.hpp>

namespace utilities {

/**
 * Classe generale che implementa una coda condivisa da piu' thread
 */
	template<typename T>
	class shared_queue : public singleton<shared_queue<T>> {
		std::deque<T> data;
		std::mutex lk;
		std::condition_variable cv;

		friend class singleton<shared_queue<T>>;

		shared_queue(){};
	public:

		/**
		 * Accoda un elemento nella coda
		 * @param Elemento da accodare nella coda
		 */
		void enqueue(const T obj) {
			LOGF;
			std::lock_guard<std::mutex> guard(lk);
			data.push_back(obj);
			LOGD("FileName: " << utf8_encode(std::wstring(obj->FileName, obj->FileNameLength / sizeof(wchar_t))) << " Action: "<< (int)obj->Action);
			cv.notify_all();
		}

		/**
		 * Elimina il primo elemento dalla coda e lo ritorna
		 * @return Elemento estratto dalla coda
		 */
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
