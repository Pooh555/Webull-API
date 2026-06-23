cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0072 NEW)

project(Webull-SDK-Demo VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(CURL REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)

add_executable(Webull-SDK-Demo 
    src/main.cpp

    src/core/application.cpp
)

target_include_directories(Webull-SDK-Demo
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../lib
)

target_link_libraries(Webull-SDK-Demo 
    PRIVATE 
        Webull::SDK
)