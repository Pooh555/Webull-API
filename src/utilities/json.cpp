#include <utilities/json.hpp>

#include <spdlog/spdlog.h>

#include <fstream>

namespace wdk::utilities {

nlohmann::json read(const std::filesystem::path& input_path) {
    if (!std::filesystem::exists(input_path)) {
        spdlog::error("[Utilities] JSON file path does not exist: {}", input_path.string());
        return nlohmann::json();
    }

    std::ifstream in(input_path);

    if (!in.is_open()) {
        spdlog::error("[Utilities] Failed to open file stream for {}", input_path.string());
        return nlohmann::json();
    }

    nlohmann::json json_data;

    try {
        in >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("[Utilities] JSON syntax parse error in {}: {}", input_path.string(), e.what());
        return nlohmann::json();
    } catch (const std::exception& e) {
        spdlog::error("[Utilities] Unexpected exception reading {}: {}", input_path.string(), e.what());
        return nlohmann::json();
    }

    return json_data;
}

void write(const nlohmann::json& json, const std::filesystem::path& output_path) {
    try {
        if (output_path.has_parent_path()) {
            std::filesystem::create_directories(output_path.parent_path());
        }

        std::ofstream out(output_path);

        if (!out.is_open()) {
            spdlog::error("[Utilities] Failed to open file stream for {}", output_path.string());
            return;
        }

        out << std::setw(4) << json;
    } catch (const std::exception& e) {
        spdlog::error("[Utilities] Exception writing {}: {}", output_path.string(), e.what());
    }
}

std::string get_string_from_json(const nlohmann::json& json_obj, const char* key) {
    if (json_obj.contains(key) && json_obj[key].is_string()) {
        return json_obj[key].get<std::string>();
    }

    spdlog::debug("[Utilities] Failed to parse JSON key or object. The object might be intentionally null.");

    return "";
}

double get_double_from_json(const nlohmann::json& json_obj, const char* key) {
    if (!json_obj.contains(key) || json_obj[key].is_null()) return 0.0;

    if (json_obj[key].is_number()) return json_obj[key].get<double>();
    if (json_obj[key].is_string()) {
        const std::string str   { json_obj[key].get<std::string>() };
        double            value { 0.0 };

        auto [ptr, ec] { std::from_chars(str.data(), str.data() + str.size(), value) };
        
        if (ec == std::errc{}) return value;
    }
    
    return 0.0;
}

size_t get_size_t_from_json(const nlohmann::json& json_obj, const char* key) {
    if (!json_obj.contains(key) || json_obj[key].is_null()) return 0uz;
    if (json_obj[key].is_number_integer())                  return json_obj[key].get<size_t>();
    if (json_obj[key].is_string()) {
        const std::string str   { json_obj[key].get<std::string>() };
        size_t            value { 0uz };

        auto [ptr, ec] { std::from_chars(str.data(), str.data() + str.size(), value) };
        
        if (ec == std::errc{}) return value;
    }
    
    return 0uz;
}

}