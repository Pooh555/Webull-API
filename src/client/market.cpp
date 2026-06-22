#include <client/market.hpp>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <format>
#include <memory>

namespace wdk::client {

MarketClient::MarketClient(
          wdk::core::CurlPool&    pool,
          wdk::core::ThreadPool&  thread_pool,
    const wdk::core::Credentials& credentials,
          std::string_view       host,
          std::string_view       token)
    : pool_(pool),
      thread_pool_(thread_pool),
      credentials_(credentials),
      host_(host),
      token_(token) {}

wdk::utilities::Response MarketClient::fetch_tick_data(
    const MarketRequest& request) {
    
    std::string path { TICK_PATH };

    bool first_parameter { true };
    auto append_parameter = [&](std::string_view key, std::string_view value) {
        if (value.empty()) return;
        path           += first_parameter ? '?' : '&';
        path           += std::format("{}={}", key, value);
        first_parameter = false;
    };
    append_parameter("symbol", request.symbol);
    append_parameter("category", request.category);
    
    if (request.count.has_value()) {
        append_parameter("count", std::to_string(request.count.value()));
    }

    append_parameter("trading_session", request.trading_session);

    return wdk::utilities::execute_request(
        pool_,
        credentials_,
        host_,
        path,
        wdk::utilities::HttpMethod::GET,
        "",
        token_
    );
}

std::future<wdk::utilities::Response> MarketClient::fetch_tick_data_async(const MarketRequest& request) {
    std::string path { TICK_PATH };

    bool first_parameter { true };
    auto append_parameter = [&](std::string_view key, std::string_view value) {
        if (value.empty()) return;
        path           += first_parameter ? '?' : '&';
        path           += std::format("{}={}", key, value);
        first_parameter = false;
    };

    append_parameter("symbol", request.symbol);
    append_parameter("category", request.category);
    
    if (request.count.has_value()) {
        append_parameter("count", std::to_string(request.count.value()));
    }
    
    append_parameter("trading_session", request.trading_session);
    
    return execute_request_async(path, wdk::utilities::HttpMethod::GET);
}

std::future<wdk::utilities::Response> MarketClient::execute_request_async(
    std::string                path,
    wdk::utilities::HttpMethod method,
    std::string                body_str) {
    return thread_pool_.enqueue(
        [this, path = std::move(path), method, body_str = std::move(body_str)]() {
            return wdk::utilities::execute_request(
                pool_,
                credentials_,
                host_,
                path,
                method,
                body_str,
                token_
            );
        }
    );
}

}