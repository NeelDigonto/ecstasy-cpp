#include <fmt/core.h>

#include <ecstasy/ecstasy.hpp>
#include <ecstasy/scene.hpp>
#include <iostream>

int main(int argc, char** argv) {
    fmt::print("Hello, world!\n");
    // Ecstasy::adapter();
    ecstasy::app app{"Clear Color"};
    // const auto scene = app.createScene();
    while (!app.shouldClose())
        app.animate();
}

// void testa() { Ecstasy::display(); }