#include <ecstasy/ecstasy.hpp>
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {

    ecstasy::app app("Sandbox");
    app.setScene("sandbox");
    app.setClearColor({0.9, 0.1, 0.2, 1.0});

    while (!app.shouldClose()) {

        app.animate();

        /* std::wcout << "Time difference = " << app.getLastAnimationTime<std::chrono::microseconds>() << "us"
                   << std::endl; */
    }
}
