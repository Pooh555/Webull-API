#include <core/curl_pool.hpp>

#include <spdlog/spdlog.h>

namespace wdk::core {

void CurlPool::share_lock(CURL*, curl_lock_data, curl_lock_access, void* userptr) {
    auto* mutex = static_cast<std::mutex*>(userptr);
    mutex->lock();
}

void CurlPool::share_unlock(CURL*, curl_lock_data, void* userptr) {
    auto* mutex = static_cast<std::mutex*>(userptr);
    mutex->unlock();
}

CurlPool::CurlPool(size_t pool_size) {
    share_handle_ = curl_share_init();
    if (share_handle_) {
        curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curl_share_setopt(share_handle_, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
        curl_share_setopt(share_handle_, CURLSHOPT_LOCKFUNC, share_lock);
        curl_share_setopt(share_handle_, CURLSHOPT_UNLOCKFUNC, share_unlock);
        curl_share_setopt(share_handle_, CURLSHOPT_USERDATA, &share_mutex_);
    }

    for (size_t i { 0uz }; i < pool_size; ++i) {
        CURL* handle = curl_easy_init();
        if (handle) {
            handles_.push(handle);
        } else {
            spdlog::critical("[CurlPool] Failed to initialize a CURL handle");
        }
    }
    
    spdlog::info("[CurlPool] Initialized connection pool with {} handles and shared DNS/SSL cache", handles_.size());
}

CurlPool::~CurlPool() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
    }
    condition_.notify_all();

    while (!handles_.empty()) {
        curl_easy_cleanup(handles_.front());
        handles_.pop();
    }

    if (share_handle_) curl_share_cleanup(share_handle_);
}

CurlPool::CurlHandle CurlPool::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return !handles_.empty() || shutdown_; });

    if (shutdown_ || handles_.empty()) {
        return CurlHandle(nullptr, [](CURL*) {});
    }

    CURL* handle = handles_.front();
    handles_.pop();

    curl_easy_reset(handle);

    if (share_handle_) {
        curl_easy_setopt(handle, CURLOPT_SHARE, share_handle_);
    }
    
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPIDLE, 60L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPINTVL, 30L);
    curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);

    return CurlHandle(handle, [this](CURL* h) { this->release(h); });
}
void CurlPool::release(CURL* handle) {
    if (!handle) return;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        handles_.push(handle);
    }
    
    condition_.notify_one();
}

}