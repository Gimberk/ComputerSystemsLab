#include "thread_pool.h"

thread_pool::thread_pool(size_t threads) {
	for (size_t i = 0; i < threads; i++) {
		workers.emplace_back([this] {
			while (true) {
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(this->queue_mutex);
					this->cv.wait(lock, [this] {return this->stop || !this->tasks.empty(); });
					if (this->stop && this->tasks.empty()) return;
					task = std::move(this->tasks.front());
					this->tasks.pop();
				}
				task();
			}
		});
	}
}

thread_pool::~thread_pool() {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}

	cv.notify_all();
	for (std::thread& worker : workers) if (worker.joinable()) worker.join();
}