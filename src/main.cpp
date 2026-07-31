#include "core/Application.hpp"
#include <iostream>
#include <exception>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Starting Minecraft C++ OpenGL Engine  " << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        Minecraft::Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "[Engine Error] Unhandled Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[Engine] Shutdown complete successfully." << std::endl;
    return EXIT_SUCCESS;
}
