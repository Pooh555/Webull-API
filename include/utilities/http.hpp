#pragma once

#include "core/curl_pool.hpp"
#include "core/secret.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <future>  

namespace wdk::utilities {

enum class HttpMethod : bool {
    GET  = false,
    POST = true
};

struct Response {
    long        http_code { 0L };  
    std::string message   { "" };
};

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

[[nodiscard]] Response execute_request(
          wdk::core::CurlPool& pool,
    const wdk::core::Secret&   secret,
          std::string_view     host,
          std::string_view     path,
          HttpMethod           method,
          std::string_view     body_str = "",
          std::string_view     token    = "");
[[nodiscard]] std::future<Response> execute_request_async(
          wdk::core::CurlPool& pool,
    const wdk::core::Secret&   secret,
          std::string_view     host,
          std::string_view     path,
          HttpMethod           method,
          std::string          body_str = "",  
          std::string          token    = "");
[[nodiscard]] curl_slist* generate_headers(
    const wdk::core::Secret& secret,
          std::string_view   timestamp = "",
          std::string_view   nonce     = "",
          std::string_view   signature = "",
          std::string_view   token     = "");

}