#include "trading.hpp"

#include "utilities/utilities.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <memory>

namespace trading {
    
std::string get_account_list(
          CURL*             curl,
    const Secret&           secret,
    const std::string_view& host,
    const std::string&      token) {
    if (curl == nullptr) {
        spdlog::error("[Trading] Passed a null curl pointer to get_account_list()");
        return "";
    }
    
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);

    std::string timestamp = utilities::get_utc_timestamp();
    std::string nonce     = utilities::generate_nonce(26uz);
    
    std::vector<std::pair<std::string, std::string>> query_parameters {};
    static constexpr std::string_view ACCOUNT_LIST_PATH = "/openapi/account/list";

    std::string signature = utilities::generate_openapi_signature(
        curl, 
        secret.get_key(), 
        secret.get_secret(), 
        nonce, 
        timestamp, 
        host, 
        ACCOUNT_LIST_PATH, 
        query_parameters, 
        ""
    );

    std::string url = "https://" + std::string(host) + std::string(ACCOUNT_LIST_PATH);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

    curl_slist* raw_headers = nullptr;
    raw_headers = utilities::generate_headers(
        raw_headers, 
        secret, 
        timestamp, 
        nonce, 
        signature,
        static_cast<std::string>(token));
        
    auto header_guard = std::unique_ptr<curl_slist, void(*)(curl_slist*)>(
        raw_headers, 
        [](curl_slist* h) { curl_slist_free_all(h); }
    );

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_guard.get());
    
    std::string response_message { "" };

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utilities::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_message);

    CURLcode response_code = curl_easy_perform(curl);

    if (response_code == CURLE_OK) {
        long int http_code { 0L };

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (http_code == 200L) {
            spdlog::info("[Trading] Successfully retrieved Webull account collection mappings");
            
            try {
                auto json_response = nlohmann::json::parse(response_message);
                spdlog::info("[Trading] Account List Response:\n{}", json_response.dump(4));
            } catch (const nlohmann::json::parse_error& e) {
                spdlog::warn("[Trading] Failed to parse JSON account list response: {}", e.what());
                spdlog::info("[Trading] Raw Response: {}", response_message);
            }
        } else {
            spdlog::error("[Trading] Account list fetch rejected. HTTP {}: {}", http_code, response_message);
        }
    } else {
        spdlog::error("[Trading] Curl routing execution failed: {}", curl_easy_strerror(response_code));
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);

    return response_message;
}

std::string preview_order(
    CURL*            curl, 
    const Secret&    secret, 
    std::string_view host, 
    std::string_view token,
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
    std::string_view stop_price) { 
    if (curl == nullptr) {
        spdlog::error("[Trading] Passed a null curl pointer to preview_order()");
        return "";
    }
    
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);

    std::string timestamp = utilities::get_utc_timestamp();
    std::string nonce     = utilities::generate_nonce(26uz);
    
    nlohmann::json order_item {
        {"combo_type",              std::string(combo_type)},
        {"client_order_id",         std::string(client_order_id)},
        {"instrument_type",         std::string(instrument_type)},
        {"market",                  std::string(market)},
        {"symbol",                  std::string(symbol)},
        {"order_type",              std::string(order_type)},
        {"entrust_type",            std::string(entrust_type)},
        {"support_trading_session", std::string(support_trading_session)},
        {"time_in_force",           std::string(time_in_force)},
        {"side",                    std::string(side)}
    };

    if (!quantity.empty())    order_item["quantity"]    = std::string(quantity);
    if (!limit_price.empty()) order_item["limit_price"] = std::string(limit_price);
    if (!stop_price.empty())  order_item["stop_price"]  = std::string(stop_price);

    nlohmann::json root_payload {
        {"account_id", std::string(account_id)},
        {"new_orders", nlohmann::json::array({order_item})}
    };

    std::string body_str = root_payload.dump();
    std::vector<std::pair<std::string, std::string>> query_parameters {};

    static constexpr std::string_view ORDER_PREVIEW_PATH = "/openapi/trade/order/preview";

    std::string signature = utilities::generate_openapi_signature(
        curl, 
        secret.get_key(), 
        secret.get_secret(), 
        nonce, 
        timestamp, 
        host, 
        ORDER_PREVIEW_PATH, 
        query_parameters, 
        body_str
    );

    std::string url = "https://" + std::string(host) + std::string(ORDER_PREVIEW_PATH);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

    curl_slist* raw_headers = nullptr;
    raw_headers = utilities::generate_headers(
        raw_headers, 
        secret, 
        timestamp, 
        nonce, 
        signature,
        static_cast<std::string>(token));

    auto header_guard = std::unique_ptr<curl_slist, void(*)(curl_slist*)>(
        raw_headers, 
        [](curl_slist* h) { curl_slist_free_all(h); }
    );

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_guard.get());
    
    std::string response_message { "" };

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utilities::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_message);

    CURLcode response_code = curl_easy_perform(curl);

    if (response_code == CURLE_OK) {
        long int http_code { 0L };

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (http_code == 200L) {
            spdlog::info("[Trading] Successfully retrieved order preview calculations");
            try {
                auto json_response = nlohmann::json::parse(response_message);
                spdlog::info("[Trading] Order Preview Response:\n{}", json_response.dump(4));
            } catch (const nlohmann::json::parse_error& e) {
                spdlog::warn("[Trading] Failed to parse JSON order preview response: {}", e.what());
                spdlog::info("[Trading] Raw Response: {}", response_message);
            }
        } else {
            spdlog::error("[Trading] Order preview rejected. HTTP {}: {}", http_code, response_message);
        }
    } else {
        spdlog::error("[Trading] Curl transmission failed: {}", curl_easy_strerror(response_code));
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, nullptr);

    return response_message;
}

void place_order() {}
void modify_order() {}
void cancel_order() {}

} 