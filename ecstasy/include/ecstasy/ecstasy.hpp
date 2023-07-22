#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <concepts>
#include <entt/entt.hpp>
#include <Eigen/Dense>

// auto create a default scene
class GLFWwindow;

namespace filament {
class Engine;
class SwapChain;
class Renderer;
class Camera;
class View;
class Scene;
class Skybox;
} // namespace filament

namespace ecstasy {

template <class T>
concept AnimationTime = std::same_as<T, std::chrono::nanoseconds> ||
                        std::same_as<T, std::chrono::microseconds> ||
                        std::same_as<T, std::chrono::milliseconds> ||
                        std::same_as<T, std::chrono::seconds>;

class app {

  public:
  private:
    std::string app_name_;
    std::uint32_t window_width_;
    std::uint32_t window_height_;
    Eigen::Vector4d clear_color_{0.5, 0.5, 0.5, 1.0};
    std::chrono::steady_clock::time_point last_animation_start_timestamp_{};
    std::chrono::steady_clock::duration last_animation_time_{};

    entt::registry registry_;

    GLFWwindow* window_;

    filament::Engine* filament_engine_;
    filament::SwapChain* filament_swapchain_;
    filament::Renderer* renderer_;
    filament::Camera* camera_;
    filament::View* view_;
    filament::Scene* scene_;
    filament::Skybox* skybox_;

    // WGPUAdapter webgpu_adapter_;
    // WGPUDevice webgpu_device_;
    // WGPUInstance webgpu_instance_;
    // WGPUCommandEncoder webgpu_encoder_;
    // WGPUQueue webgpu_queue_;
    // WGPUSwapChain webgpu_swapchain_;
    // WGPURenderPipeline render_pipeline_;

  public:
    app(std::string _app_name = "Ecstasy", std::uint32_t _window_width = 1280U,
        std::uint32_t _window_height = 720U);
    app(const app& _app) = delete;
    app(app&& _app) = delete;
    // ecstasy::scene createScene();
    void setClearColor(const Eigen::Vector4d& _clear_color) noexcept;
    bool shouldClose() const noexcept;
    template <ecstasy::AnimationTime T = std::chrono::microseconds>
    typename T::rep getLastAnimationTime() const noexcept;
    void printUsage(std::chrono::steady_clock::duration _duration);
    void animate();
    ~app();
};
} // namespace ecstasy
