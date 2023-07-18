// #include<GLFW / glfw3.h>

#include <cassert>
#include <iostream>
#include <vector>

#include <ecstasy/ecstasy.hpp>
#include <Eigen/Dense>
#include <chrono>
// https : // github.com/BinomialLLC/basis_universal
ecstasy::app::app(std::string _app_name, std::uint32_t _window_width,
                  std::uint32_t _window_height) {
    app_name_ = _app_name;
    window_height_ = _window_height;
    window_width_ = _window_width;

    // if (!glfwInit()) {
    //     std::cerr << "Could not initialize GLFW!" << std::endl;
    //     return;
    // }
    //
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // window_ = glfwCreateWindow(window_width_, window_height_,
    // app_name_.c_str(),
    //                           NULL, NULL);
    // if (!window_) {
    //    std::cerr << "Could not open window!" << std::endl;
    //    return;
    //}
}

void ecstasy::app::setClearColor(const Eigen::Vector4d& _clear_color) noexcept {
    clear_color_ = _clear_color;
};

bool ecstasy::app::shouldClose() const noexcept {
    // return glfwWindowShouldClose(window_);
    return false;
}

/* ecstasy::scene ecstasy::app::createScene() {
    // registry_.emplace<>;
    std::cout << "Scene Created! " << std::endl;
} */

void ecstasy::app::animate() {
    // glfwPollEvents();

    // Get the texture where to draw the next frame
}

ecstasy::app::~app() {
    // glfwDestroyWindow(window_);
    // glfwTerminate();
}
