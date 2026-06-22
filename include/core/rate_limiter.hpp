#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace wdk::core {

class RateLimiter {
public:
    RateLimiter(size_t max_tokens, std::chrono::milliseconds refill_interval);
    ~RateLimiter() = default;

    void acquire(size_t tokens = 1uz);
private:
    size_t                                 max_tokens_;
    size_t                                 current_tokens_;
    std::chrono::milliseconds              refill_interval_;
    std::chrono::steady_clock::time_point  next_refill_;

    std::mutex              mutex_;
    std::condition_variable cv_;
};

}