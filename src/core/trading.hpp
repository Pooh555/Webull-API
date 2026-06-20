#pragma once

#include "secret.hpp"
#include "core/curl_pool.hpp"

#include <optional>
#include <string>
#include <string_view>

struct OrderRequest {
    std::string           account_id              = "";            
    std::string           combo_type              = "";  
    std::string           client_order_id         = "";  
    std::string           instrument_type         = "";  
    std::string           market                  = "";  
    std::string           symbol                  = "";  
    std::string           order_type              = "";  
    std::string           entrust_type            = "";  
    std::string           support_trading_session = "";  
    std::string           time_in_force           = "";  
    std::string           side                    = "";  
    std::optional<double> quantity                = std::nullopt;  
    std::optional<double> limit_price             = std::nullopt;  
    std::optional<double> stop_price              = std::nullopt;  
};

class TradingClient {
public:
    TradingClient(
              CurlPool&        pool,
        const Secret&          secret, 
              std::string_view host, 
              std::string_view token);
    ~TradingClient() = default;

    std::string preview_order(const OrderRequest& request);
    std::string place_order(const OrderRequest& request);
    std::string modify_order(const OrderRequest& request);
    std::string cancel_order(const OrderRequest& request);

    [[nodiscard]] std::string get_account_id();
    [[nodiscard]] std::string get_account_list();
private:
    static constexpr std::string_view ACCOUNT_LIST_PATH  = "/openapi/account/list";
    static constexpr std::string_view PREVIEW_ORDER_PATH = "/openapi/trade/order/preview";
    static constexpr std::string_view PLACE_ORDER_PATH   = "/openapi/trade/order/place";
    static constexpr std::string_view MODIFY_ORDER_PATH  = "/openapi/trade/order/replace";
    static constexpr std::string_view CANCEL_ORDER_PATH  = "/openapi/trade/order/cancel";

          std::string account_id = "";
          CurlPool&   pool_;  
    const Secret&     secret_;
          std::string host_      = "";
          std::string token_     = "";
};