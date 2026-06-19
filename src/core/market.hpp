#pragma once

#include "token.hpp"
#include "secret/secret.hpp"

#include <curl/curl.h>

#include <string>
#include <string_view>
#include <vector>

class Market {
public:
    Market()  = default;
    ~Market() = default;

    Market(const Market&)            = delete;
    Market& operator=(const Market&) = delete;

    struct TickData {
        std::string symbol          = "";
        std::string instrument_id   = "";
        // std::string price           = "";
        // std::string open            = "";
        // std::string high            = "";
        // std::string low             = "";
        std::string volume          = "";
        std::string side            = "";
        // std::string change          = "";
        // std::string change_ratio    = "";
        // std::string pre_close       = "";
        // std::string last_trade_time = "";
        std::string trading_session = "";
    };

    std::vector<TickData> fetch_tick_data(
              CURL*             curl, 
        const Secret&           secret, 
        const std::string&      token, 
        const std::string&      symbol,
        const std::string&      category,
        const std::string&      count,
        const std::string&      session,
        const std::string_view& host);
private:
    static constexpr std::string_view TICK_PATH = "/openapi/market-data/stock/tick";
};