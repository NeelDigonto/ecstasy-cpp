#pragma once
#include <string>
#include <chrono>
#include <concepts>
#include <fmt/core.h>
#include <thread>
#include <Eigen/Dense>

// auto create a default scene
struct GLFWwindow;

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

class InputController;
class EditorController;
namespace scene {
class scene;
}

template <typename T, typename U> inline T cast(U& _data) noexcept { return *reinterpret_cast<T*>(&_data); }

/* template <typename T, typename U> inline T cast(U&& _data) noexcept {
    return *reinterpret_cast<T*>(&_data);
} */

extern Eigen::IOFormat CommaInitFmt;
extern Eigen::IOFormat CleanFmt;
extern Eigen::IOFormat OctaveFmt;
extern Eigen::IOFormat HeavyFmt;

template <class T>
concept AnimationTime =
    std::same_as<T, std::chrono::nanoseconds> || std::same_as<T, std::chrono::microseconds> ||
    std::same_as<T, std::chrono::milliseconds> || std::same_as<T, std::chrono::seconds>;

class app {

  public:
  private:
    std::string app_name_;
    Eigen::Vector4d clear_color_{0.5, 0.5, 0.5, 1.0};
    std::chrono::steady_clock::time_point last_animation_start_timestamp_{};
    std::chrono::steady_clock::time_point last_animation_end_timestamp_{};
    std::chrono::steady_clock::duration last_animation_time_{};

    GLFWwindow* window_;

    filament::Engine* filament_engine_;
    filament::SwapChain* filament_swapchain_;
    filament::Renderer* renderer_;
    scene::scene* scene_;

    InputController* input_controller_;

  public:
    app(std::string _app_name = "Ecstasy", std::uint32_t _window_width = 2920U,
        std::uint32_t _window_height = 2080U);
    app(const app& _app) = delete;
    app(app&& _app) = delete;
    void setScene(std::string _scene_name = "sandbox");
    void setClearColor(const Eigen::Vector4d& _clear_color) noexcept;
    bool shouldClose() const noexcept;
    InputController* getInputController() noexcept;
    GLFWwindow* getGLFWWindow() noexcept;
    template <ecstasy::AnimationTime T = std::chrono::microseconds>
    typename T::rep getLastAnimationTime() const noexcept;
    void printUsage(std::chrono::steady_clock::duration _duration);
    void animate();
    ~app();
};
} // namespace ecstasy
