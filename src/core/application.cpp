#include "application.hpp"

#include "trading.hpp"
#include "utilities/utilities.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

Application::Application() {
    curl   = std::make_unique<Curl>();
    secret = std::make_unique<Secret>(SECRET_PATH);
    token  = std::make_unique<Token>(TOKEN_PATH, curl->get_handle(), *secret.get(), HOST);
    market = std::make_unique<Market>();
}

Application::~Application() {}

void Application::run() {
    spdlog::info("[Application] Querying dynamic user account targets...");

    std::string account_list_json = trading::get_account_list(
        curl->get_handle(), 
        *secret.get(), 
        HOST, 
        token->get_handle()
    );

    std::string extracted_account_id = "";

    try {
        if (!account_list_json.empty()) {
            auto parsed_response = nlohmann::json::parse(account_list_json);
            
            if (parsed_response.is_array() && !parsed_response.empty()) {
                extracted_account_id = parsed_response[0].value("account_id", "");
            } else if (parsed_response.contains("data") && parsed_response["data"].is_array() && !parsed_response["data"].empty()) {
                extracted_account_id = parsed_response["data"][0].value("account_id", "");
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("[Application] Failed to parse account listing JSON metrics: {}", e.what());
        return;
    }

    if (extracted_account_id.empty()) {
        spdlog::error("[Application] Could not determine a valid account ID target mapping. Aborting preview request.");
        return;
    }

    spdlog::info("[Application] Target localized. Executing preview for account: {}", extracted_account_id);

    std::string client_order_id = utilities::generate_nonce(26uz);

    std::string preview_json = trading::preview_order(
        curl->get_handle(),            // CURL* handle
        *secret.get(),                 // Const Secret& reference
        HOST,                          // String view of the base host domain
        token->get_handle(),           // Retrieved OAuth Access Token string
        extracted_account_id,          // Valid extracted account_id parameter target
        "NORMAL",                      // combo_type: Single standalone order execution
        client_order_id,               // client_order_id: Unique string reference
        "EQUITY",                      // instrument_type: US Stocks/ETFs
        "US",                          // market: Trading venue region
        "AAPL",                        // symbol: Ticker
        "LIMIT",                       // order_type: LIMIT pricing order logic block
        "QTY",                         // entrust_type: Quantity based purchase
        "CORE",                        // support_trading_session: Standard execution hours
        "DAY",                         // time_in_force: Current trading day duration limits
        "BUY",                         // side: Purchase order action
        "10",                          // quantity: units metrics target size
        "185.50",                      // limit_price string metric
        ""                             // stop_price: Empty
    );
}