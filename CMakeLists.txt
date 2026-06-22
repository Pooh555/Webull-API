cmake_minimum_required(VERSION 3.20.0)
cmake_policy(SET CMP0072 NEW)

project(Webull-SDK  VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(CURL REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)

add_library(Webull-SDK STATIC
    src/client/market.cpp
    src/client/trading.cpp
    
    src/core/credentials.cpp
    src/core/curl_pool.cpp
    src/core/rate_limiter.cpp
    src/core/thread_pool.cpp
    src/core/token.cpp
    
    src/utilities/cryptography.cpp
    src/utilities/http.cpp
    src/utilities/json.cpp
    src/utilities/openapi.cpp
    src/utilities/time.cpp
)

add_library(Webull::SDK ALIAS Webull-SDK)

target_include_directories(Webull-SDK
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
)

target_link_libraries(Webull-SDK 
    PRIVATE 
        spdlog::spdlog
        CURL::libcurl
        fmt::fmt
        OpenSSL::Crypto
)

option(WEBULL_BUILD_EXAMPLES "Build example applications" ON)

if(WEBULL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()