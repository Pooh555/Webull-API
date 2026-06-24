#pragma once

#include <utilities/http.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace wdk::data {

struct TickData {
    std::string symbol           { "" };
    std::string instrument_id    { "" };
    std::string volume           { "" };
    std::string side             { "" };
    std::string trading_sessions { "" };
};

struct SnapshotData {
    std::string instrument_id                 { "" };
    std::string pre_close                     { "" };
    std::string change_ratio                  { "" };
    std::string symbol                        { "" };

    size_t last_trade_time                    { 0uz };

    std::string price                         { "" };
    std::string open                          { "" };
    std::string close                         { "" };
    std::string high                          { "" };
    std::string low                           { "" };
    std::string volume                        { "" };
    std::string change                        { "" };

    std::string ask                           { "" };
    std::string ask_size                      { "" };
    std::string bid                           { "" };
    std::string bid_size                      { "" };

    std::string extend_hour_last_price        { "" };
    std::string extend_hour_high              { "" };
    std::string extend_hour_low               { "" };
    std::string extend_hour_change            { "" };
    std::string extend_hour_change_ratio      { "" };
    std::string extend_hour_volume            { "" };

    size_t extend_hour_last_trade_time        { 0uz };

    std::string ovn_price                     { "" };
    std::string ovn_high                      { "" };
    std::string ovn_low                       { "" };
    std::string ovn_volume                    { "" };
    std::string ovn_change                    { "" };
    std::string ovn_change_ratio              { "" };

    size_t ovn_last_trade_time                { 0uz };

    std::string ovn_ask                       { "" };
    std::string ovn_ask_size                  { "" };
    std::string ovn_bid                       { "" };
    std::string ovn_bid_size                  { "" };
};

struct QuoteLevel {
    std::string price { "" };
    std::string size  { "" };
};

struct QuotesData {
    std::string             symbol        { "" };
    std::string             instrument_id { "" };
    size_t                  quote_time    { 0uz };
    
    std::vector<QuoteLevel> asks          {};
    std::vector<QuoteLevel> bids          {};
};

struct FootPrintBar {
    std::string time            { "" };
    std::string trading_session { "" };
    std::string total           { "" };
    std::string delta           { "" };
    std::string buy_total       { "" };
    std::string sell_total      { "" };

    std::map<std::string, std::string> buy_detail  {};
    std::map<std::string, std::string> sell_detail {};
};

struct FootPrintData {
    std::string               symbol        { "" };
    std::string               instrument_id { "" };
    std::vector<FootPrintBar> results       {};
};

struct Bar {
    std::string time            { "" };
    double      open            { 0.0 };
    double      high            { 0.0 };
    double      low             { 0.0 };
    double      close           { 0.0 };
    double      volume          { 0.0 };
    std::string trading_session { "" };
};

struct HistoricalBarsData {
    std::string      symbol        { "" };
    std::string      instrument_id { "" };
    std::vector<Bar> bars          {};
};

[[nodiscard]] TickData                        convert_response_to_tick_data(wdk::utilities::Response response);
[[nodiscard]] SnapshotData                    convert_response_to_snapshot_data(wdk::utilities::Response response);
[[nodiscard]] std::vector<SnapshotData>       convert_response_to_snapshot_vector(wdk::utilities::Response response);
[[nodiscard]] QuotesData                      convert_response_to_quotes_data(wdk::utilities::Response response);
[[nodiscard]] std::vector<FootPrintData>      convert_response_to_footprint_vector(wdk::utilities::Response response);
[[nodiscard]] std::vector<HistoricalBarsData> convert_response_to_historical_bars_vector(wdk::utilities::Response response);

// Internal helper methods
[[nodiscard]] SnapshotData  parse_snapshot_node(const nlohmann::json& node);
[[nodiscard]] FootPrintData parse_footprint_node(const nlohmann::json& node);
[[nodiscard]] Bar           parse_bar_node(const nlohmann::json& node);

}