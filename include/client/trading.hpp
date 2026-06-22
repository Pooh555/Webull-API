#pragma once

#include "core/curl_pool.hpp"
#include "core/thread_pool.hpp"
#include "core/credentials.hpp"
#include "utilities/http.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace wdk::client {

struct OrderRequest {
    std::string           account_id              { "" };
    std::string           combo_type              { "" };
    std::string           client_order_id         { "" };
    std::string           instrument_type         { "" };
    std::string           market                  { "" };
    std::string           symbol                  { "" };
    std::string           order_type              { "" };
    std::string           entrust_type            { "" };
    std::string           trading_session         { "" };
    std::string           time_in_force           { "" };
    std::string           side                    { "" };
    std::optional<double> quantity                { std::nullopt };
    std::optional<double> limit_price             { std::nullopt };
    std::optional<double> stop_price              { std::nullopt };  
};

struct QueryRequest {
    std::string           symbols            { "" };
    std::string           category           { "" };
    std::string           status             { "" };
    std::string           last_instrument_id { "" };
    std::string           account_id         { "" };
    std::string           start_date         { "" };
    std::optional<size_t> page_size          { std::nullopt };
    std::string           last_client_id     { "" };
    std::string           client_order_id    { "" };
};

class TradingClient {
public:
    TradingClient(
              wdk::core::CurlPool&    pool,
              wdk::core::ThreadPool&  thread_pool,
        const wdk::core::Credentials& credentials, 
              std::string_view        host, 
              std::string_view        token);
    ~TradingClient() = default;

    [[nodiscard]] wdk::utilities::Response              preview_order(const OrderRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> preview_order_async(const OrderRequest& request);
    [[nodiscard]] wdk::utilities::Response              place_order(const OrderRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> place_order_async(const OrderRequest& request);
    [[nodiscard]] wdk::utilities::Response              modify_order(const OrderRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> modify_order_async(const OrderRequest& request);
    [[nodiscard]] wdk::utilities::Response              cancel_order(const OrderRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> cancel_order_async(const OrderRequest& request);

    [[nodiscard]] wdk::utilities::Response              fetch_stock_instrument(const QueryRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_stock_instrument_async(const QueryRequest& request);
    [[nodiscard]] wdk::utilities::Response              fetch_order_history(const QueryRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_order_history_async(const QueryRequest& request);
    [[nodiscard]] wdk::utilities::Response              fetch_open_order(const QueryRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_open_order_async(const QueryRequest& request);
    [[nodiscard]] wdk::utilities::Response              fetch_order_detail(const QueryRequest& request);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_order_detail_async(const QueryRequest& request);
    
    [[nodiscard]] wdk::utilities::Response              fetch_account_balance(const std::string& account_id);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_account_balance_async(const std::string& account_id);
    [[nodiscard]] wdk::utilities::Response              fetch_account_position(const std::string& account_id);
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_account_position_async(const std::string& account_id);
    
    [[nodiscard]] std::string                           get_account_id();
    [[nodiscard]] wdk::utilities::Response              fetch_account_list();
    [[nodiscard]] std::future<wdk::utilities::Response> fetch_account_list_async();
private:    
    static constexpr std::string_view PREVIEW_ORDER_PATH { "/openapi/trade/order/preview" };
    static constexpr std::string_view PLACE_ORDER_PATH   { "/openapi/trade/order/place" };
    static constexpr std::string_view MODIFY_ORDER_PATH  { "/openapi/trade/order/replace" };
    static constexpr std::string_view CANCEL_ORDER_PATH  { "/openapi/trade/order/cancel" };

    static constexpr std::string_view STOCK_INSTRUMMENT_PATH { "/openapi/instrument/stock/list" };
    static constexpr std::string_view ORDER_HISTORY_PATH     { "/openapi/trade/order/history" };
    static constexpr std::string_view OPEN_ORDER_PATH        { "/openapi/trade/order/open" };
    static constexpr std::string_view ORDER_DETAIL_PATH      { "/openapi/trade/order/detail" };

    static constexpr std::string_view ACCOUNT_LIST_PATH     { "/openapi/account/list" };
    static constexpr std::string_view ACCOUNT_BALANCE_PATH  { "/openapi/assets/balance" };
    static constexpr std::string_view ACCOUNT_POSITION_PATH { "/openapi/assets/positions" };
    
          std::string             account_id_  { "" };
          wdk::core::CurlPool&    pool_;  
          wdk::core::ThreadPool&  thread_pool_;
    const wdk::core::Credentials& credentials_;
          std::string             host_        { "" };
          std::string             token_       { "" };

    [[nodiscard]] std::future<wdk::utilities::Response> execute_request_async(
        std::string                path,
        wdk::utilities::HttpMethod method,
        std::string                body_str = "");
};

}