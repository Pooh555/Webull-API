#pragma once

#include "core/curl_pool.hpp"
#include "core/secret.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace utilities::http {

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
          CurlPool&        pool, 
    const Secret&          secret, 
          std::string_view host, 
          std::string_view path, 
          HttpMethod       method,
          std::string_view body_str = "",
          std::string_view token    = "");
          
[[nodiscard]] curl_slist* generate_headers(
    const Secret&          secret,
          std::string_view timestamp,
          std::string_view nonce,
          std::string_view signature,
          std::string_view token);

}