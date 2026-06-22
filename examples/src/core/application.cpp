#include "application.hpp"

#include <client/trading.hpp>
#include <utilities/cryptography.hpp>
#include <utilities/http.hpp>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <future> 

Application::Application() {
    spdlog::set_level(spdlog::level::info);

    static constexpr size_t connections { 10uz };

    curl_pool   = std::make_unique<wdk::core::CurlPool>(connections);
    thread_pool = std::make_unique<wdk::core::ThreadPool>();
    credentials = std::make_unique<wdk::core::Credentials>(CREDENTIALS_PATH);
    token       = std::make_unique<wdk::core::Token>(TOKEN_PATH, *curl_pool.get(), *credentials.get(), HOST);
}

void Application::run() {
    market_demo();
    trading_demo();
}    

void Application::market_demo() {
    // Initialize a market client for fetching market data
    wdk::client::MarketClient market_client(
        *curl_pool,
        *thread_pool,
        *credentials,
        HOST,
        token->get_handle()
    );

    // Fetch tick data
    spdlog::info("[Application] Fetching tick data...");

    std::future<wdk::utilities::Response> tick_future = market_client.fetch_tick_data_async({ 
        .symbol           { "AAPL" },
        .category         { "US_STOCK" },
        .count            { 2uz  },
        .trading_sessions { "PRE" }
    });
    wdk::utilities::Response tick_data = tick_future.get();

    if (tick_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched tick data:\n{}", nlohmann::json::parse(tick_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch tick data:\n{}", nlohmann::json::parse(tick_data.message).dump(4));
    }

    // Fetch snapshot data
    spdlog::info("[Application] Fetching snapshot data...");

    std::future<wdk::utilities::Response> snapshot_future = market_client.fetch_snapshot_data_async({ 
        .symbols                { "AAPL,NVDA" },
        .category               { "US_STOCK" },
        .extended_hour_required { false },
        .overnight_required     { false }
    });
    wdk::utilities::Response snapshot_data = snapshot_future.get();

    if (snapshot_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched snapshot data:\n{}", nlohmann::json::parse(snapshot_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch snapshot data:\n{}", nlohmann::json::parse(snapshot_data.message).dump(4));
    }

    // Fetch quotes data
    spdlog::info("[Application] Fetching quotes data...");

    std::future<wdk::utilities::Response> quotes_future = market_client.fetch_quotes_data_async({ 
        .symbol                 { "AAPL" },
        .category               { "US_STOCK" },
        .depth                  { 1u },
        .overnight_required     { false }
    });
    wdk::utilities::Response quotes_data = quotes_future.get();

    if (quotes_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched quotes data:\n{}", nlohmann::json::parse(quotes_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch quotes data:\n{}", nlohmann::json::parse(quotes_data.message).dump(4));
    }

    // Fetch footprint data
    spdlog::info("[Application] Fetching footprint data...");

    std::future<wdk::utilities::Response> footprint_future = market_client.fetch_footprint_data_async({ 
        .symbols                 { "AAPL,NVDA" },
        .category                { "US_STOCK" },
        .timespan                { "M5" },
        .count                   { 3uz },
        .real_time_required      { false },
        .trading_sessions        { "PRE" }
    });
    wdk::utilities::Response footprint_data = footprint_future.get();

    if (footprint_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched footprint data:\n{}", nlohmann::json::parse(footprint_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch footprint data:\n{}", nlohmann::json::parse(footprint_data.message).dump(4));
    }

    // Fetch historical bar (single symbol) data
    spdlog::info("[Application] Fetching historical bars (single symbol) data...");

    std::future<wdk::utilities::Response> historical_bars_future = market_client.fetch_historical_bars_data_async({ 
        .symbol                  { "AAPL" },
        .category                { "US_STOCK" },
        .timespan                { "M5" },
        .count                   { 3uz },
        .real_time_required      { false },
        .trading_sessions        { "PRE" }
    });
    wdk::utilities::Response historical_bars_data = historical_bars_future.get();

    if (historical_bars_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched historical bars (single symbol) data:\n{}", nlohmann::json::parse(historical_bars_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch historical bars (single symbol) data:\n{}", nlohmann::json::parse(historical_bars_data.message).dump(4));
    }

    // Fetch historical bar (batch) data
    spdlog::info("[Application] Fetching historical bars (batch)) data...");

    std::future<wdk::utilities::Response> historical_batch_bars_future = market_client.fetch_historical_batch_bars_data_async({ 
        .symbols                 { "AAPL,NVDA" },
        .category                { "US_STOCK" },
        .timespan                { "M5" },
        .count                   { 3uz },
        .real_time_required      { false },
        .trading_sessions        { "PRE" }
    });
    wdk::utilities::Response historical_batch_bars_data = historical_batch_bars_future.get();

    if (historical_batch_bars_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched historical bars (batch) data:\n{}", nlohmann::json::parse(historical_batch_bars_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch historical bars (batch) data:\n{}", nlohmann::json::parse(historical_batch_bars_data.message).dump(4));
    }
}

void Application::trading_demo() {
    // Initialize a trading client for handling order operations
    wdk::client::TradingClient client(
        *curl_pool, 
        *thread_pool,
        *credentials.get(), 
        HOST, 
        token->get_handle()
    );

    // Extract account id
    const std::string account_id = client.get_account_id();

    // Fetch stock instrument
    spdlog::info("[Application] Fetching stock instrument...");
    
    std::future<wdk::utilities::Response> stock_instrument_future = client.fetch_stock_instrument_async({
        .symbols            { "AAPL,NVDA" },
        .category           { "US_STOCK" },
        .status             { "OC" },
        .last_instrument_id { "951007842" },
        .page_size          { 10uz }
    });
    wdk::utilities::Response stock_instrument_data = stock_instrument_future.get();
    
    if (stock_instrument_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched stock instrument:\n {}", nlohmann::json::parse(stock_instrument_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetched stock instrument:\n {}", nlohmann::json::parse(stock_instrument_data.message).dump(4));
    }

    // Fetch account balance
    spdlog::info("[Application] Dispatching balance request...");

    std::future<wdk::utilities::Response> account_balance_future  = client.fetch_account_balance_async(account_id);
    wdk::utilities::Response              account_balance         = account_balance_future.get();

    if (account_balance.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched account balance:\n {}", nlohmann::json::parse(account_balance.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch account balance:\n {}", nlohmann::json::parse(account_balance.message).dump(4));
    }
    
    // Fetch account position
    spdlog::info("[Application] Dispatching position request...");

    std::future<wdk::utilities::Response> account_position_future = client.fetch_account_position_async(account_id);
    wdk::utilities::Response              account_position        = account_position_future.get();
    
    if (account_position.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched account positions:\n {}", nlohmann::json::parse(account_position.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetch account positions:\n {}", nlohmann::json::parse(account_position.message).dump(4));
    }

    // Retrive order id (nonce)
    std::string client_order_id = wdk::utilities::generate_nonce();

    // Place an order
    spdlog::info("[Application] Dispatching order placement...");

    std::future<wdk::utilities::Response> place_order_future = client.place_order_async({
        .account_id              { account_id },          
        .combo_type              { "NORMAL" },                      
        .client_order_id         { client_order_id },               
        .instrument_type         { "EQUITY" },                      
        .market                  { "US" },
        .symbol                  { "NVDA" },
        .order_type              { "LIMIT" },
        .entrust_type            { "QTY" },
        .trading_session         { "CORE" },                        
        .time_in_force           { "DAY" },
        .side                    { "BUY" },
        .quantity                { 1.0 },
        .limit_price             { 100.05 },
        .stop_price              { std::nullopt }
    });
    wdk::utilities::Response place_order = place_order_future.get();
    
    if (place_order.http_code == 200L) {
        spdlog::info("[Application] Successfully placed order:\n {}", nlohmann::json::parse(place_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to place order:\n {}", nlohmann::json::parse(place_order.message).dump(4));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Modify an order
    spdlog::info("[Application] Modifying placed order reference: {}", client_order_id);
    
    std::future<wdk::utilities::Response> modify_order_future = client.modify_order_async({
        .account_id      { account_id },
        .client_order_id { client_order_id },
        .time_in_force   { "DAY" }, 
        .quantity        { 1.0 },
        .limit_price     { 100.15 },
        .stop_price      { std::nullopt }
    });
    wdk::utilities::Response modify_order = modify_order_future.get();

    if (modify_order.http_code == 200L) {
        spdlog::info("[Application] Successfully modified order:\n {}", nlohmann::json::parse(modify_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to modify order:\n {}", nlohmann::json::parse(modify_order.message).dump(4));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Cancel an order
    spdlog::info("[Application] Cancelling order reference: {}", client_order_id);
    
    std::future<wdk::utilities::Response> cancel_order_future = client.cancel_order_async({
        .account_id      { account_id },
        .client_order_id { client_order_id },
    });
    wdk::utilities::Response cancel_order = cancel_order_future.get();
    
    if (cancel_order.http_code == 200L) {
        spdlog::info("[Application] Successfully canceled order:\n {}", nlohmann::json::parse(cancel_order.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to cancel order:\n {}", nlohmann::json::parse(cancel_order.message).dump(4));
    }

    // Fetch order history
    spdlog::info("[Application] Fetching order history...");
    
    std::future<wdk::utilities::Response> order_history_future = client.fetch_order_history_async({
        .account_id     { account_id },
        .start_date     { "2026-05-01" },
        .page_size      { 10uz },
        .last_client_id { client_order_id }
    });
    wdk::utilities::Response order_history_data = order_history_future.get();
    
    if (order_history_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched order history:\n {}", nlohmann::json::parse(order_history_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetched order history:\n {}", nlohmann::json::parse(order_history_data.message).dump(4));
    }

    // Fetch open order
    spdlog::info("[Application] Fetching open order...");
    
    std::future<wdk::utilities::Response> open_order_future = client.fetch_open_order_async({
        .account_id     { account_id },
        .page_size      { 10uz },
        .last_client_id { client_order_id }
    });
    wdk::utilities::Response open_order_data = open_order_future.get();
    
    if (open_order_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched open order:\n {}", nlohmann::json::parse(open_order_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetched open order:\n {}", nlohmann::json::parse(open_order_data.message).dump(4));
    }

    // Fetch order detail
    spdlog::info("[Application] Fetching order detail...");
    
    std::future<wdk::utilities::Response> order_detail_future = client.fetch_order_detail_async({
        .account_id      { account_id },
        .client_order_id { client_order_id }
    });
    wdk::utilities::Response order_detail_data = order_detail_future.get();
    
    if (order_detail_data.http_code == 200L) {
        spdlog::info("[Application] Successfully fetched order detail:\n {}", nlohmann::json::parse(order_detail_data.message).dump(4));
    } else {
        spdlog::error("[Application] Failed to fetched order detail:\n {}", nlohmann::json::parse(order_detail_data.message).dump(4));
    }
}