#include <ecstasy/ecstasy.hpp>
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    ecstasy::app app("Clear Color");
    app.setClearColor({0.9, 0.1, 0.2, 1.0});

    while (!app.shouldClose()) {
        std::chrono::steady_clock::time_point begin =
            std::chrono::steady_clock::now();

        app.animate();

        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        std::wcout << "Time difference = "
                   << std::chrono::duration_cast<std::chrono::microseconds>(
                          end - begin)
                          .count()
                   << "[us]" << std::endl;
    }
}
