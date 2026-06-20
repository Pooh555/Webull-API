#pragma once

#include "curl.hpp"
#include "market.hpp"
#include "secret.hpp"
#include "token.hpp"

#include <memory>

class Application {
public:
    Application();
    ~Application();

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    void run();
private:
    // static constexpr std::string_view HOST        = "th-api.uat.webullbroker.com"; // Test endpoint
    static constexpr std::string_view HOST        = "api.webull.co.th";            // Production endpoint
    static constexpr std::string_view TOKEN_PATH  = "../res//token.json";
    static constexpr std::string_view SECRET_PATH = "../res/secret.json";

    std::unique_ptr<Curl>   curl;
    std::unique_ptr<Secret> secret;
    std::unique_ptr<Token>  token;
    std::unique_ptr<Market> market;
};