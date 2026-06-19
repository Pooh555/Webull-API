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
        spdlog::error("[Application] Could not determine a valid account ID target mapping. Aborting request pipeline.");
        return;
    }

    spdlog::info("[Application] Target localized: {}. Executing order placement for SSG...", extracted_account_id);

    std::string client_order_id = utilities::generate_nonce(26uz);

    std::string place_json = trading::place_order(
        curl->get_handle(),            
        *secret.get(),                 
        HOST,                          
        token->get_handle(),           
        extracted_account_id,          
        "NORMAL",                      
        client_order_id,               
        "EQUITY",                      
        "US",                          
        "SSG",                        
        "LIMIT",                       
        "QTY",                         
        "CORE",                        
        "DAY",                         
        "BUY",                         
        "1",                          
        "11.20",                      
        ""                             
    );

    if (place_json.empty()) {
        spdlog::error("[Application] Order placement failed. Aborting demo workflow sequence.");
        return;
    }

    spdlog::info("[Application] Modifying placed order reference: {}", client_order_id);

    std::string modify_json = trading::modify_order(
        curl->get_handle(),
        *secret.get(),
        HOST,
        token->get_handle(),
        extracted_account_id,
        client_order_id,
        "1",
        "11.30",
        "",
        "DAY"
    );

    if (modify_json.empty()) {
        spdlog::error("[Application] Order modification failed. Aborting demo workflow sequence.");
        return;
    }

    spdlog::info("[Application] Cancelling order reference: {}", client_order_id);

    std::string cancel_json = trading::cancel_order(
        curl->get_handle(),
        *secret.get(),
        HOST,
        token->get_handle(),
        extracted_account_id,
        client_order_id
    );
}