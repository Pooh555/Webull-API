#include "core/application.hpp"

#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    Application application;

    try {
        application.run();
    } catch (const std::exception& e) {
        spdlog::critical("[Main] Failed to run the application: {}", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}