#pragma once
#include <cstdint>
#include <string>
#include <ecstasy/fd.h>
#include <entt/entt.hpp>
#include <ecstasy/scene.hpp>
//   #include <Eigen/Dense>

namespace ecstasy {

class app {
  private:
    std::string app_name_;
    std::uint32_t window_width_;
    std::uint32_t window_height_;

    entt::registry registry_;

    GLFWwindow* window_;
    WGPUAdapter webgpu_adapter_;
    WGPUDevice webgpu_device_;
    WGPUInstance webgpu_instance_;
    WGPUSwapChain webgpu_swapchain_;

  public:
    app(std::string _app_name = "Ecstasy", std::uint32_t _window_width = 1280U,
        std::uint32_t _window_height = 720U);
    ecstasy::scene createScene();
    void run();
    ~app();
};
} // namespace ecstasy
