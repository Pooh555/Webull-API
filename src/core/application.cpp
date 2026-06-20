#include "application.hpp"

#include "trading.hpp"
#include "utilities/cryptography.hpp"

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

    std::string client_order_id = utilities::cryptography::generate_nonce(26uz);
    std::string place_json      = client.place_order({
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

    if (place_json.empty()) {
        spdlog::error("[Application] Order placement failed. Aborting demo workflow sequence");
        return;
    }

    spdlog::info("[Application] Modifying placed order reference: {}", client_order_id);

    std::string modify_json = client.modify_order({
        .account_id      = extracted_account_id,
        .client_order_id = client_order_id,
        .time_in_force   = "DAY",
        .quantity        = 1.0,
        .limit_price     = 11.30,
        .stop_price      = std::nullopt
    });

    if (modify_json.empty()) {
        spdlog::error("[Application] Order modification failed. Aborting demo workflow sequence");
        return;
    }

    spdlog::info("[Application] Cancelling order reference: {}", client_order_id);

    std::string cancel_json = client.cancel_order({
        .account_id      = extracted_account_id,
        .client_order_id = client_order_id,
    });
}