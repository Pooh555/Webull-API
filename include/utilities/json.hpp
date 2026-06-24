#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>

namespace wdk::utilities {

[[nodiscard]] nlohmann::json read(const std::filesystem::path& input_path);
              void           write(const nlohmann::json& json, const std::filesystem::path& output_path);

[[nodiscard]] std::string get_string_from_json(const nlohmann::json& json_obj, const char* key);
[[nodiscard]] double      get_double_from_json(const nlohmann::json& json_obj, const char* key);
[[nodiscard]] size_t      get_size_t_from_json(const nlohmann::json& json_obj, const char* key);
}