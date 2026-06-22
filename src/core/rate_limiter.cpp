#include <core/rate_limiter.hpp>

namespace wdk::core {

RateLimiter::RateLimiter(size_t max_tokens, std::chrono::milliseconds refill_interval)
    : max_tokens_(max_tokens), 
      current_tokens_(max_tokens),
      refill_interval_(refill_interval), 
      next_refill_(std::chrono::steady_clock::now() + refill_interval) {}

void RateLimiter::acquire(size_t tokens) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    while (current_tokens_ < tokens) {
        auto now = std::chrono::steady_clock::now();
        
        if (now >= next_refill_) {
            current_tokens_ = max_tokens_;
            next_refill_    = now + refill_interval_;
        } else {
            cv_.wait_until(lock, next_refill_);
        }
    }
    
    current_tokens_ -= tokens;
}

}