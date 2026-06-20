#pragma once

#include "secret.hpp"
#include "core/curl_pool.hpp" 

#include <string>
#include <string_view>

class Token {
public:
    Token(
        const std::filesystem::path& token_path,
              CurlPool&              pool,   
        const Secret&                secret,
        const std::string_view&      host);
    ~Token() = default;
    
    Token(const Token&)            = delete;
    Token& operator=(const Token&) = delete;

    Token(Token&&)            noexcept = default;
    Token& operator=(Token&&) noexcept = default;

    void generate(
              CurlPool&         pool,        
        const Secret&           secret, 
        const std::string_view& host);

    void verify(
              CurlPool&         pool,        
        const Secret&           secret, 
        const std::string_view& host);

    [[nodiscard]] std::string get_handle() const { return token; }
    [[nodiscard]] std::string get_status() const { return status; }
    [[nodiscard]] bool        is_valid()   const { return !token.empty(); }
private:
    static constexpr std::string_view CREATE_PATH = "/openapi/auth/token/create";
    static constexpr std::string_view VERIFY_PATH = "/openapi/auth/token/check";

    std::string token  = "";
    std::string status = "";
};