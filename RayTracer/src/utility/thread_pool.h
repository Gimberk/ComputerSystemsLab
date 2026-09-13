#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class thread_pool {
public:
	thread_pool(size_t threads);

	// I 100% see why people call out C++ for being overly verbose. What the heck am I looking at? All that just so this function could be generic? Really??
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
		using return_type = typename std::invoke_result<F, Args...>::type;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			if (stop) throw std::runtime_error("enqeue on stopped thread pool");
			tasks.emplace([task]() {(*task)(); });
		}

		cv.notify_one();
		return res;
	}

	~thread_pool();

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;

	std::mutex queue_mutex;
	std::condition_variable cv;
	bool stop = false;
};