#pragma once

#include <condition_variable>
#include <future>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace wdk::core {

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;
private:
    std::vector<std::jthread>                   workers_;
    std::queue<std::move_only_function<void()>> tasks_;
    
    std::mutex              queue_mutex_;
    std::condition_variable condition_;
    bool                    stop_ { false };
};

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;

    std::packaged_task<return_type()> task(
        std::bind_front(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task.get_future();

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (stop_) {
            throw std::runtime_error("[ThreadPool] Enqueue on stopped pool");
        }
        
        tasks_.emplace([task = std::move(task)]() mutable {
            task();
        });
    }

    condition_.notify_one();

    return result;
}

}