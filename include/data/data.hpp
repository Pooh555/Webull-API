#pragma once

#include <utilities/http.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <map>

namespace wdk::data {

struct TickData {
    std::string symbol           { "" };
    std::string instrument_id    { "" };
    double      volume           { 0.0 };
    std::string side             { "" };
    std::string trading_sessions { "" };
};

struct SnapshotData {
    std::string instrument_id                 { "" };
    double      pre_close                     { 0.0 };
    double      change_ratio                  { 0.0 };
    std::string symbol                        { "" };

    size_t      last_trade_time               { 0uz };

    double      price                         { 0.0 };
    double      open                          { 0.0 };
    double      close                         { 0.0 };
    double      high                          { 0.0 };
    double      low                           { 0.0 };
    double      volume                        { 0.0 };
    double      change                        { 0.0 };

    double      ask                           { 0.0 };
    double      ask_size                      { 0.0 };
    double      bid                           { 0.0 };
    double      bid_size                      { 0.0 };

    double      extend_hour_last_price        { 0.0 };
    double      extend_hour_high              { 0.0 };
    double      extend_hour_low               { 0.0 };
    double      extend_hour_change            { 0.0 };
    double      extend_hour_change_ratio      { 0.0 };
    double      extend_hour_volume            { 0.0 };

    size_t      extend_hour_last_trade_time   { 0uz };

    double      ovn_price                     { 0.0 };
    double      ovn_high                      { 0.0 };
    double      ovn_low                       { 0.0 };
    double      ovn_volume                    { 0.0 };
    double      ovn_change                    { 0.0 };
    double      ovn_change_ratio              { 0.0 };

    size_t      ovn_last_trade_time           { 0uz };

    double      ovn_ask                       { 0.0 };
    double      ovn_ask_size                  { 0.0 };
    double      ovn_bid                       { 0.0 };
    double      ovn_bid_size                  { 0.0 };
};

struct QuoteLevel {
    double      price { 0.0 };
    double      size  { 0.0 };
};

struct QuotesData {
    std::string             symbol        { "" };
    std::string             instrument_id { "" };
    size_t                  quote_time    { 0uz };
    
    std::vector<QuoteLevel> asks {};
    std::vector<QuoteLevel> bids {};
};

struct FootPrintBar {
    std::string time            { "" };
    std::string trading_session { "" };
    double      total           { 0.0 };
    double      delta           { 0.0 };
    double      buy_total       { 0.0 };
    double      sell_total      { 0.0 };

    std::map<std::string, double> buy_detail  {};
    std::map<std::string, double> sell_detail {};
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