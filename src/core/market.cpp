#include "market.hpp"
#include "utilities/utilities.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <memory>

std::vector<Market::TickData> Market::fetch_tick_data(
          CURL*                 curl, 
    const Secret&               secret, 
    const std::string&          token, 
    const std::string&          symbol,
    const std::string&          category,
    const std::string&          count,
    const std::string&          session,
    const std::string_view&     host) {
    std::vector<Market::TickData> ticks {};

    if (curl == nullptr) {
        spdlog::error("[Market] Passed a null curl pointer to fetch_tick_data()");
        return ticks;
    }
    
    std::string timestamp = utilities::get_utc_timestamp();
    std::string nonce     = utilities::generate_nonce(26uz);
    
    std::vector<std::pair<std::string, std::string>> query_parameters {
        {"category", category},
        {"count", count},
        {"symbol", symbol},
        {"trading_sessions", session}
    };

    std::string signature = utilities::generate_openapi_signature(
        curl, secret.get_key(), secret.get_secret(), nonce, timestamp, host, TICK_PATH, query_parameters, ""
    );

    char* escaped_category = curl_easy_escape(curl, category.c_str(), static_cast<int>(category.length()));
    char* escaped_count    = curl_easy_escape(curl, count.c_str(), static_cast<int>(count.length()));
    char* escaped_symbol   = curl_easy_escape(curl, symbol.c_str(), static_cast<int>(symbol.length()));
    char* escaped_session  = curl_easy_escape(curl, session.c_str(), static_cast<int>(session.length()));

    std::string url = "https://" + std::string(host) + std::string(TICK_PATH) + 
                      "?category=" + std::string(escaped_category) + 
                      "&count=" + std::string(escaped_count) + 
                      "&symbol=" + std::string(escaped_symbol) + 
                      "&trading_sessions=" + std::string(escaped_session);

    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

    curl_slist* raw_headers = nullptr;
    raw_headers = curl_slist_append(raw_headers, "Accept: application/json");
    raw_headers = curl_slist_append(raw_headers, "User-Agent: WebullBot/1.0 (C++23 Client)");
    raw_headers = curl_slist_append(raw_headers, ("x-app-key: " + secret.get_key()).c_str());
    raw_headers = curl_slist_append(raw_headers, ("x-timestamp: " + timestamp).c_str());
    raw_headers = curl_slist_append(raw_headers, "x-signature-version: 1.0");
    raw_headers = curl_slist_append(raw_headers, "x-signature-algorithm: HMAC-SHA1");
    raw_headers = curl_slist_append(raw_headers, ("x-signature-nonce: " + nonce).c_str());
    raw_headers = curl_slist_append(raw_headers, ("x-access-token: " + token).c_str());
    raw_headers = curl_slist_append(raw_headers, "x-version: v2");
    raw_headers = curl_slist_append(raw_headers, ("x-signature: " + signature).c_str());

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
            spdlog::info("[Market] Successfully fetched tick market data");

            try {
                auto json_response = nlohmann::json::parse(response_message);

                spdlog::debug("[Market] Tick market data payload parsed successfully");

                nlohmann::json json_ticks;
                
                if (json_response.is_array()) {
                    json_ticks = json_response;
                } else if (json_response.contains("data") && json_response["data"].is_array()) {
                    json_ticks = json_response["data"];
                } else if (json_response.contains("ticks") && json_response["ticks"].is_array()) {
                    json_ticks = json_response["ticks"];
                }

                if (!json_ticks.empty()) {
                    ticks.reserve(json_ticks.size());

                    for (const auto& item : json_ticks) {
                        TickData data {
                            .symbol          = item.value("symbol", symbol),
                            .instrument_id   = item.value("instrument_id", item.value("instrumentId", "")),
                            .volume          = item.value("volume", ""),
                            .side            = item.value("side", ""),
                            .trading_session = item.value("trading_session", item.value("tradingSession", ""))
                        };
                        
                        ticks.push_back(data);
                    }
                } else {
                    spdlog::warn("[Market] API response did not contain a recognizable data array.");
                }
            } catch (const nlohmann::json::parse_error& e) {
                spdlog::warn("[Market] Failed to parse JSON response: {}", e.what());
            }
        } else {
            spdlog::error("[Market] API rejected request. HTTP {}: {}", http_code, response_message);
        }
    } else {
        spdlog::error("[Market] Curl request execution failed: {}", curl_easy_strerror(response_code));
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);

    return ticks;
}