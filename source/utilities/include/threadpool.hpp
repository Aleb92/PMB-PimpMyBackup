#ifndef SOURCE_UTILITIES_THREADPOOL_HPP_
#define SOURCE_UTILITIES_THREADPOOL_HPP_

#include <functional>
#include <future>
#include <thread>


namespace utilities {

	/**
	 * template thread_pool, che abbiamo preferito rispetto ad una classe
	 * virtuale perchè così si evita l'eventuale passaggio da una v-table.
	 *
	 */
	template <typename POLICY, typename A, typename...T>
	class thread_pool : public thread_pool<POLICY, T...> {
	public:
		static std::future<A> execute(std::function<A()> f) {
			return POLICY::dispatch(f);
		}
	};

	template <typename POLICY,typename A>
	class thread_pool<POLICY, A> {
	public:

		static std::future<A> execute(std::function<A()> f) {
			return POLICY::dispatch(f);
		}
	};

	/**
	 * Questa classe implementa a tutti gli effetti la politica della
	 * thread pool. In questo caso lascia che sia il c++ a decidere
	 * come eseguire i compiti affidatigli.
	 * 		però è meglio di utilizzare l'operatore () perchè se no ci si
	 * 		confonde poi con i functor/callable e non mi piace.
	 */
	struct default_threa_pool_policy {
		template <typename T>
		static std::future<T> dispatch(std::function<T()> f) {
			return std::async(f);
		}
	};

}
#endif
