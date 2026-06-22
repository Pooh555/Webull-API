#include <core/thread_pool.hpp>

namespace wdk::core {

ThreadPool::ThreadPool(size_t threads) {
    for (size_t i { 0uz }; i < threads; ++i) {
        workers_.emplace_back([this](std::stop_token st) {
            while (!st.stop_requested()) {
                std::move_only_function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this, &st] { 
                        return stop_ || !tasks_.empty() || st.stop_requested(); 
                    });
                    
                    if ((stop_ || st.stop_requested()) && tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
}

}