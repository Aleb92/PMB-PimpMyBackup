#ifndef SOURCE_UTILITIES_THREADPOOL_HPP_
#define SOURCE_UTILITIES_THREADPOOL_HPP_

#include <functional>
#include <future>
#include <thread>


namespace utilities {

	template <typename POLICY, typename A, typename...T>
	class thread_pool : public thread_pool<POLICY, T...> {
	public:
		static std::future<A> execute(std::function<A()> f) {
			return POLICY(f);
		}
	};

	template <typename POLICY,typename A>
	class thread_pool<POLICY, A> {
	public:

		static std::future<A> execute(std::function<A()> f) {
			return POLICY(f);
		}
	};

	struct default_threa_pool_policy {
		template <typename T>
		static std::future<T> operator()(std::function<T()> f) {
			return std::async(f);
		}
	};

}
#endif
