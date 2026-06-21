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

}