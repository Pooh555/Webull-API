#include "market.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <format>
#include <memory>

MarketClient::MarketClient(
          CurlPool&        pool,
    const Secret&          secret,
          std::string_view host,
          std::string_view token)
    : pool_(pool),
      secret_(secret),
      host_(host),
      token_(token) {}

utilities::http::Response MarketClient::fetch_tick_data(
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

    return utilities::http::execute_request(
        pool_,
        secret_,
        host_,
        path,
        utilities::http::HttpMethod::GET,
        "",
        token_
    );
}

std::future<utilities::http::Response> MarketClient::fetch_tick_data_async(const MarketRequest& request) {
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

    return utilities::http::execute_request_async(
        pool_,
        secret_,
        host_,
        path,
        utilities::http::HttpMethod::GET,
        "",
        token_
    );
}