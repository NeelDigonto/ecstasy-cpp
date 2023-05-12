#include <ecstasy/ecstasy.hpp>

int main(int argc, char** argv) {
    ecstasy::app app("Clear Color");
    app.setClearColor({0.9, 0.1, 0.2, 1.0});
    while (!app.shouldClose())
        app.animate();
}
