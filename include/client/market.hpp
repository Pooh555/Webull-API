#pragma once

#include "core/curl_pool.hpp"
#include "core/secret.hpp"
#include "core/token.hpp"
#include "utilities/http.hpp"

#include <future>
#include <string>
#include <string_view>
#include <vector>

struct MarketRequest {
    std::string           symbol          { "" };
    std::string           category        { "" };
    std::optional<size_t> count           { 0uz };
    std::string           trading_session { "" };
};

struct TickData {
    std::string symbol          { "" };
    std::string instrument_id   { "" };
    // std::string price           { "" };
    // std::string open            { "" };
    // std::string high            { "" };
    // std::string low             { "" };
    std::string volume          { "" };
    std::string side            { "" };
    // std::string change          { "" };
    // std::string change_ratio    { "" };
    // std::string pre_close       { "" };
    // std::string last_trade_time { "" };
    std::string trading_session { "" };
};

class MarketClient {
public:
    MarketClient(
              CurlPool&        pool,
        const Secret&          secret, 
              std::string_view host, 
              std::string_view token);
    ~MarketClient() = default;

    MarketClient(const MarketClient&)            = delete;
    MarketClient& operator=(const MarketClient&) = delete;

    [[nodiscard]] wdk::utilities::Response fetch_tick_data(const MarketRequest& request);

    [[nodiscard]] std::future<wdk::utilities::Response> fetch_tick_data_async(const MarketRequest& request);
private:
    static constexpr std::string_view TICK_PATH = "/openapi/market-data/stock/tick";

          CurlPool&   pool_;  
    const Secret&     secret_;
          std::string host_      { "" };
          std::string token_     { "" };
};