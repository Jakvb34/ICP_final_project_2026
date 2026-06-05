#include "app.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        App app;
        if (!app.init()) {
            std::cerr << "Application initialization failed.\n";
            return 1;
        }
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Fatal unknown error.\n";
        return 1;
    }

    return 0;
}
