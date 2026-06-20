#include "application.hpp"

#include "trading.hpp"
#include "utilities/cryptography.hpp"
#include "utilities/http.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

Application::Application() {
    spdlog::set_level(spdlog::level::debug);

    static constexpr size_t connections { 10uz };

    curl_pool = std::make_unique<CurlPool>(connections);
    secret    = std::make_unique<Secret>(SECRET_PATH);
    token     = std::make_unique<Token>(TOKEN_PATH, *curl_pool.get(), *secret.get(), HOST);
    market    = std::make_unique<Market>();
}

Application::~Application() {}

void Application::run() {
    TradingClient client(
        *curl_pool, 
        *secret.get(), 
        HOST, 
        token->get_handle()
    );

    const std::string extracted_account_id = client.get_account_id();

    spdlog::info("[Application] Fetching account balance for mapping: {}", extracted_account_id);

    utilities::http::Response account_balance = client.fetch_account_balance(extracted_account_id);

    if (account_balance.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched account balance:\n {}", nlohmann::json::parse(account_balance.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch account balance:\n {}", nlohmann::json::parse(account_balance.message).dump(4));
    }

    spdlog::info("[Application] Fetching account positions for mapping: {}", extracted_account_id);

    utilities::http::Response account_position = client.fetch_account_position(extracted_account_id);

    if (account_position.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched account positions:\n {}", nlohmann::json::parse(account_position.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch account positions:\n {}", nlohmann::json::parse(account_position.message).dump(4));
    }

    std::string client_order_id = utilities::cryptography::generate_nonce(26uz);

    utilities::http::Response place_order = client.place_order({
        .account_id              = extracted_account_id,          
        .combo_type              = "NORMAL",                      
        .client_order_id         = client_order_id,               
        .instrument_type         = "EQUITY",                      
        .market                  = "US",                          
        .symbol                  = "SSG",                        
        .order_type              = "LIMIT",                       
        .entrust_type            = "QTY",                         
        .support_trading_session = "CORE",                        
        .time_in_force           = "DAY",                         
        .side                    = "BUY",                         
        .quantity                = 1.0,                          
        .limit_price             = 11.20,                      
        .stop_price              = std::nullopt
    });

    if (place_order.http_code == 200L) {
        spdlog::info("[Application] Successfully placed order:\n {}", nlohmann::json::parse(place_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to placed order:\n {}", nlohmann::json::parse(place_order.message).dump(4));
    }

    spdlog::info("[Application] Modifying placed order reference: {}", client_order_id);

    utilities::http::Response modify_order = client.modify_order({
        .account_id      = extracted_account_id,
        .client_order_id = client_order_id,
        .time_in_force   = "DAY",
        .quantity        = 1.0,
        .limit_price     = 11.30,
        .stop_price      = std::nullopt
    });

    if (modify_order.http_code == 200L) {
        spdlog::info("[Application] Successfully modified order:\n {}", nlohmann::json::parse(modify_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to modified order:\n {}", nlohmann::json::parse(modify_order.message).dump(4));
    }

    spdlog::info("[Application] Cancelling order reference: {}", client_order_id);

    utilities::http::Response cancel_order = client.cancel_order({
        .account_id      = extracted_account_id,
        .client_order_id = client_order_id,
    });

    if (cancel_order.http_code == 200L) {
        spdlog::info("[Application] Successfully canceled order:\n {}", nlohmann::json::parse(cancel_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to canceled order:\n {}", nlohmann::json::parse(cancel_order.message).dump(4));
    }
}