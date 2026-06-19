#pragma once

#include "secret/secret.hpp"

#include <curl/curl.h>

#include <string>

namespace trading {

std::string get_account_list(
          CURL*             curl,
    const Secret&           secret,
    const std::string_view& host,
    const std::string&      token
);
std::string preview_order(
    CURL*            curl, 
    const Secret&    secret, 
    std::string_view host, 
    std::string_view access_token,
    std::string_view account_id,            
    std::string_view combo_type,
    std::string_view client_order_id,
    std::string_view instrument_type,
    std::string_view market,
    std::string_view symbol,
    std::string_view order_type,
    std::string_view entrust_type,
    std::string_view support_trading_session,
    std::string_view time_in_force,
    std::string_view side,
    std::string_view quantity,
    std::string_view limit_price,
    std::string_view stop_price);
void place_order();
void modify_order();
void cancel_order();

}