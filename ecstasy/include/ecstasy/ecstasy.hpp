#pragma once
#include <entt/entt.hpp>
#include <ecstasy/scene.hpp>
// #include <Eigen/Dense>

class GLFWwindow;
class WGPUInstanceImpl;
class WGPUAdapterImpl;
class WGPUDeviceImpl;
class WGPUSwapChainImpl;

typedef struct WGPUInstanceImpl* WGPUInstance;
typedef struct WGPUAdapterImpl* WGPUAdapter;
typedef struct WGPUDeviceImpl* WGPUDevice;
typedef struct WGPUSwapChainImpl* WGPUSwapChain;

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
