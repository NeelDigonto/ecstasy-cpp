#include <fmt/core.h>

#include <ecstasy/ecstasy.hpp>
#include <ecstasy/scene.hpp>
#include <iostream>

int main(int argc, char** argv) {
    fmt::print("Hello, world!\n");
    // Ecstasy::adapter();
    ecstasy::app app;
    const auto scene = app.createScene();
}

// void testa() { Ecstasy::display(); }