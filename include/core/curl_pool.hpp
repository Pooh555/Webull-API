#pragma once

#include <curl/curl.h>

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace wdk::core {

class CurlPool {
public:
    using CurlReleaser = std::function<void(CURL*)>;
    using CurlHandle   = std::unique_ptr<CURL, CurlReleaser>;

    explicit CurlPool(size_t pool_size = 10uz);
    ~CurlPool();

    CurlPool(const CurlPool&)            = delete;
    CurlPool& operator=(const CurlPool&) = delete;

    [[nodiscard]] CurlHandle acquire();

private:
    void release(CURL* handle);

    std::queue<CURL*>       handles_;
    std::mutex              mutex_;
    std::condition_variable condition_;
    bool                    shutdown_  { false };
};

}