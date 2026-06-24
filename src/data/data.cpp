#include <data/data.hpp>

#include <utilities/json.hpp>

#include <spdlog/spdlog.h>

#include <string>

namespace wdk::data {

TickData convert_response_to_tick_data(wdk::utilities::Response response) {
    TickData data {};

    if (response.message.empty()) return data;

    auto json_root = nlohmann::json::parse(response.message, nullptr, false);
    
    if (json_root.is_discarded()) return data;

    data.symbol        = wdk::utilities::get_string_from_json(json_root, "symbol");
    data.instrument_id = wdk::utilities::get_string_from_json(json_root, "instrument_id");

    if (json_root.contains("result") && json_root["result"].is_array() && !json_root["result"].empty()) {
        const auto& latest_tick = json_root["result"][0];
        data.volume           = wdk::utilities::get_double_from_json(latest_tick, "volume");
        data.side             = wdk::utilities::get_string_from_json(latest_tick, "side");
        data.trading_sessions = wdk::utilities::get_string_from_json(latest_tick, "trading_session");
    }

    return data;
}

SnapshotData convert_response_to_snapshot_data(wdk::utilities::Response response) {
    SnapshotData data {};

    if (response.message.empty()) return data;

    auto json_root { nlohmann::json::parse(response.message, nullptr, false) };

    if (json_root.is_discarded()) return data;

    const nlohmann::json* target_node = nullptr;

    if (json_root.is_array() && !json_root.empty()) {
        target_node = &json_root[0];
    } else if (json_root.contains("result") && json_root["result"].is_array() && !json_root["result"].empty()) {
        target_node = &json_root["result"][0];
    } else if (json_root.contains("data") && json_root["data"].is_array() && !json_root["data"].empty()) {
        target_node = &json_root["data"][0];
    } else if (json_root.is_object() && json_root.contains("symbol")) {
        return parse_snapshot_node(json_root);
    }

    if (target_node) {
        if (target_node->is_array() && !target_node->empty()) {
            return parse_snapshot_node((*target_node)[0]);
        }

        return parse_snapshot_node(*target_node);
    }

    spdlog::error("[Data] Schema mismatch in single-snapshot parser, raw response: {}", response.message);
    
    return data;
}

std::vector<SnapshotData> convert_response_to_snapshot_vector(wdk::utilities::Response response) {
    std::vector<SnapshotData> batch_data {};

    if (response.message.empty()) return batch_data;

    auto json_root { nlohmann::json::parse(response.message, nullptr, false) };

    if (json_root.is_discarded()) return batch_data;

    const nlohmann::json* target_array { nullptr };

    if (json_root.is_array()) {
        target_array = &json_root;
    } else if (json_root.contains("result") && json_root["result"].is_array()) {
        target_array = &json_root["result"];
    } else if (json_root.contains("data") && json_root["data"].is_array()) {
        target_array = &json_root["data"];
    }

    if (target_array) {
        for (const auto& item : *target_array) {
            if (item.is_array()) {
                for (const auto& nested_item : item) {
                    if (nested_item.is_object()) {
                        batch_data.push_back(parse_snapshot_node(nested_item));
                    }
                }
            } else if (item.is_object()) {
                batch_data.push_back(parse_snapshot_node(item));
            }
        }
    } else {
        if (json_root.is_object() && json_root.contains("symbol")) {
            batch_data.push_back(parse_snapshot_node(json_root));
        } else {
            spdlog::error("[Data] Unexpected JSON schema or business error returned by server. Raw payload:\n{}", response.message);
        }
    }

    return batch_data;
}

QuotesData convert_response_to_quotes_data(wdk::utilities::Response response) {
    QuotesData data {};

    if (response.message.empty()) return data;

    auto json_root = nlohmann::json::parse(response.message, nullptr, false);
    
    if (json_root.is_discarded()) return data;

    data.symbol        = wdk::utilities::get_string_from_json(json_root, "symbol");
    data.instrument_id = wdk::utilities::get_string_from_json(json_root, "instrument_id");
    data.quote_time    = wdk::utilities::get_size_t_from_json(json_root, "quote_time");

    if (json_root.contains("asks") && json_root["asks"].is_array()) {
        data.asks.reserve(json_root["asks"].size());

        for (const auto& item : json_root["asks"]) {
            QuoteLevel level {};

            level.price = wdk::utilities::get_double_from_json(item, "price");
            level.size  = wdk::utilities::get_double_from_json(item, "size");

            data.asks.push_back(level);
        }
    }

    if (json_root.contains("bids") && json_root["bids"].is_array()) {
        data.bids.reserve(json_root["bids"].size());

        for (const auto& item : json_root["bids"]) {
            QuoteLevel level {};

            level.price = wdk::utilities::get_double_from_json(item, "price");
            level.size  = wdk::utilities::get_double_from_json(item, "size");
            
            data.bids.push_back(level);
        }
    }

    return data;
}

std::vector<FootPrintData> convert_response_to_footprint_vector(wdk::utilities::Response response) {
    std::vector<FootPrintData> batch_data {};

    if (response.message.empty()) return batch_data;

    auto json_root = nlohmann::json::parse(response.message, nullptr, false);

    if (json_root.is_discarded()) return batch_data;

    if (json_root.is_array()) {
        batch_data.reserve(json_root.size());

        for (const auto& node : json_root) {
            if (node.is_object()) {
                batch_data.push_back(parse_footprint_node(node));
            }
        }
    }

    return batch_data;
}

std::vector<HistoricalBarsData> convert_response_to_historical_bars_vector(wdk::utilities::Response response) {
    std::vector<HistoricalBarsData> batch_data {};

    if (response.message.empty()) return batch_data;

    auto json_root = nlohmann::json::parse(response.message, nullptr, false);

    if (json_root.is_discarded()) return batch_data;

    if (json_root.is_array()) {
        if (json_root.empty()) return batch_data;

        HistoricalBarsData symbol_data {
            .symbol        = wdk::utilities::get_string_from_json(json_root[0], "symbol"),
            .instrument_id = wdk::utilities::get_string_from_json(json_root[0], "tickerId")
        };
        
        symbol_data.bars.reserve(json_root.size());

        for (const auto& node : json_root) {
            symbol_data.bars.push_back(parse_bar_node(node));
        }

        batch_data.push_back(symbol_data);
    } else if (json_root.is_object() && json_root.contains("result") && json_root["result"].is_array()) {
        for (const auto& symbol_node : json_root["result"]) {
            HistoricalBarsData symbol_data {
                .symbol        = wdk::utilities::get_string_from_json(symbol_node, "symbol"),
                .instrument_id = wdk::utilities::get_string_from_json(symbol_node, "instrument_id")
            };

            if (symbol_node.contains("result") && symbol_node["result"].is_array()) {
                symbol_data.bars.reserve(symbol_node["result"].size());

                for (const auto& bar_node : symbol_node["result"]) {
                    symbol_data.bars.push_back(parse_bar_node(bar_node));
                }
            }
            
            batch_data.push_back(symbol_data);
        }
    }

    return batch_data;
}

SnapshotData parse_snapshot_node(const nlohmann::json& node) {
    return {
        .instrument_id = wdk::utilities::get_string_from_json(node, "instrument_id"),
        .pre_close     = wdk::utilities::get_double_from_json(node, "pre_close"),
        .change_ratio  = wdk::utilities::get_double_from_json(node, "change_ratio"),
        .symbol        = wdk::utilities::get_string_from_json(node, "symbol"),
        
        .last_trade_time = wdk::utilities::get_size_t_from_json(node, "last_trade_time"),

        .price  = wdk::utilities::get_double_from_json(node, "price"),
        .open   = wdk::utilities::get_double_from_json(node, "open"),
        .close  = wdk::utilities::get_double_from_json(node, "close"),
        .high   = wdk::utilities::get_double_from_json(node, "high"),
        .low    = wdk::utilities::get_double_from_json(node, "low"),
        .volume = wdk::utilities::get_double_from_json(node, "volume"),
        .change = wdk::utilities::get_double_from_json(node, "change"),

        .ask      = wdk::utilities::get_double_from_json(node, "ask"),
        .ask_size = wdk::utilities::get_double_from_json(node, "ask_size"),
        .bid      = wdk::utilities::get_double_from_json(node, "bid"),
        .bid_size = wdk::utilities::get_double_from_json(node, "bid_size"),

        .extend_hour_last_price   = wdk::utilities::get_double_from_json(node, "extend_hour_last_price"),
        .extend_hour_high         = wdk::utilities::get_double_from_json(node, "extend_hour_high"),
        .extend_hour_low          = wdk::utilities::get_double_from_json(node, "extend_hour_low"),
        .extend_hour_change       = wdk::utilities::get_double_from_json(node, "extend_hour_change"),
        .extend_hour_change_ratio = wdk::utilities::get_double_from_json(node, "extend_hour_change_ratio"),
        .extend_hour_volume       = wdk::utilities::get_double_from_json(node, "extend_hour_volume"),
        
        .extend_hour_last_trade_time = wdk::utilities::get_size_t_from_json(node, "extend_hour_last_trade_time"),

        .ovn_price        = wdk::utilities::get_double_from_json(node, "ovn_price"),
        .ovn_high         = wdk::utilities::get_double_from_json(node, "ovn_high"),
        .ovn_low          = wdk::utilities::get_double_from_json(node, "ovn_low"),
        .ovn_volume       = wdk::utilities::get_double_from_json(node, "ovn_volume"),
        .ovn_change       = wdk::utilities::get_double_from_json(node, "ovn_change"),
        .ovn_change_ratio = wdk::utilities::get_double_from_json(node, "ovn_change_ratio"),

        .ovn_last_trade_time = wdk::utilities::get_size_t_from_json(node, "ovn_last_trade_time"),

        .ovn_ask       = wdk::utilities::get_double_from_json(node, "ovn_ask"),
        .ovn_ask_size  = wdk::utilities::get_double_from_json(node, "ovn_ask_size"),
        .ovn_bid       = wdk::utilities::get_double_from_json(node, "ovn_bid"),
        .ovn_bid_size  = wdk::utilities::get_double_from_json(node, "ovn_bid_size"),
    };
}

FootPrintData parse_footprint_node(const nlohmann::json& node) {
    FootPrintData data {
        .symbol        = wdk::utilities::get_string_from_json(node, "symbol"),
        .instrument_id = wdk::utilities::get_string_from_json(node, "instrument_id")
    };

    if (node.contains("result") && node["result"].is_array()) {
        data.results.reserve(node["result"].size());

        for (const auto& item : node["result"]) {
            FootPrintBar bar {
                .time            = wdk::utilities::get_string_from_json(item, "time"),
                .trading_session = wdk::utilities::get_string_from_json(item, "trading_session"),
                .total           = wdk::utilities::get_double_from_json(item, "total"),
                .delta           = wdk::utilities::get_double_from_json(item, "delta"),
                .buy_total       = wdk::utilities::get_double_from_json(item, "buy_total"),
                .sell_total      = wdk::utilities::get_double_from_json(item, "sell_total")
            };

            if (item.contains("buy_detail") && item["buy_detail"].is_object()) {
                for (auto& [price, volume] : item["buy_detail"].items()) {
                    if (volume.is_number()) {
                        bar.buy_detail[price] = volume.get<double>();
                    } else if (volume.is_string()) {
                        // Dynamically fall back to parsing if structural payload uses stringified numbers
                        bar.buy_detail[price] = wdk::utilities::get_double_from_json(item["buy_detail"], price.c_str());
                    }
                }
            }
            if (item.contains("sell_detail") && item["sell_detail"].is_object()) {
                for (auto& [price, volume] : item["sell_detail"].items()) {
                    if (volume.is_number()) {
                        bar.sell_detail[price] = volume.get<double>();
                    } else if (volume.is_string()) {
                        bar.sell_detail[price] = wdk::utilities::get_double_from_json(item["sell_detail"], price.c_str());
                    }
                }
            }

            data.results.push_back(bar);
        }
    }

    return data;
}

Bar parse_bar_node(const nlohmann::json& node) {
    return {
        .time            = wdk::utilities::get_string_from_json(node, "time"),
        .open            = wdk::utilities::get_double_from_json(node, "open"),
        .high            = wdk::utilities::get_double_from_json(node, "high"),
        .low             = wdk::utilities::get_double_from_json(node, "low"),
        .close           = wdk::utilities::get_double_from_json(node, "close"),
        .volume          = wdk::utilities::get_double_from_json(node, "volume"),
        .trading_session = wdk::utilities::get_string_from_json(node, "trading_session")
    };
}

} 