#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>

namespace wdk::utilities {

[[nodiscard]] nlohmann::json read(const std::filesystem::path& input_path);
              void           write(const nlohmann::json& json, const std::filesystem::path& output_path);

}