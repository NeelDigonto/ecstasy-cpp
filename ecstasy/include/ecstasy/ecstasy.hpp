#pragma once
#include <ecstasy/fd.h>
#include <entt/entt.hpp>
#include <ecstasy/scene.hpp>
//   #include <Eigen/Dense>

namespace ecstasy {

class app {
  private:
    entt::registry registry_;
    GLFWwindow* window_;
    WGPUAdapter webgpu_adapter_;
    WGPUDevice webgpu_device_;
    WGPUInstance webgpu_instance_;
    WGPUSwapChain webgpu_swapchain_;

  public:
    app();
    ecstasy::scene createScene();
    void run();
    ~app();
};
} // namespace ecstasy
