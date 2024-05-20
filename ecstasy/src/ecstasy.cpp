#include <common/common.hpp>

#include <ecstasy/ecstasy.hpp>
#include <controller/InputController.hpp>
#include <controller/EditorController.hpp>
#include <shader/simple.hpp>

#include <Eigen/Dense>
#include <fmt/core.h>

#include <vector>
#include <chrono>
#include <numbers>

#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/SwapChain.h>

#include <scene/scene.hpp>
#include <scene/sandbox.hpp>

#include <common/glfw.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Eigen::IOFormat ecstasy::CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "",
                                      " << ", ";");
Eigen::IOFormat ecstasy::CleanFmt(4, 0, ", ", "\n", "[", "]");
Eigen::IOFormat ecstasy::OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
Eigen::IOFormat ecstasy::HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");

ecstasy::app::app(std::string _app_name, std::uint32_t _window_width, std::uint32_t _window_height) {
    app_name_ = _app_name;

    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW!");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(_window_width, _window_height, app_name_.c_str(), NULL, NULL);
    if (!window_)
        throw std::runtime_error("Could not open window!");

    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    input_controller_ = new InputController(window_, {_window_width, _window_height});

    filament_engine_ = filament::Engine::create(filament::Engine::Backend::OPENGL); // Engine::Backend::VULKAN

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    auto native_window = glfwGetWin32Window(window_);
#elif (1)
    auto native_window = glfwGetX11Window(window_);
#endif

    filament_swapchain_ = filament_engine_->createSwapChain((void*)native_window);
    renderer_ = filament_engine_->createRenderer();
    renderer_resource_manager_ = new RendererResourceManager(*filament_engine_);
}

void ecstasy::app::setClearColor(const Eigen::Vector4d& _clear_color) noexcept {
    clear_color_ = _clear_color;
};

bool ecstasy::app::shouldClose() const noexcept { return glfwWindowShouldClose(window_); }

ecstasy::InputController* ecstasy::app::getInputController() noexcept { return input_controller_; }

GLFWwindow* ecstasy::app::getGLFWWindow() noexcept { return window_; }

void ecstasy::app::setScene(std::string _scene_name) {
    if (_scene_name == "sandbox") {
        scene_ = new scene::sandbox(*filament_engine_, *renderer_, *renderer_resource_manager_,
                                    *input_controller_);
        scene_->build();
    }

    else {
        throw new std::runtime_error("Unkown Scene.");
    }
}

template <ecstasy::AnimationTime T> typename T::rep ecstasy::app::getLastAnimationTime() const noexcept {
    return std::chrono::duration_cast<T>(last_animation_time_).count();
}
template std::chrono::nanoseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::nanoseconds>() const noexcept;
template std::chrono::microseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::microseconds>() const noexcept;
template std::chrono::milliseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::milliseconds>() const noexcept;
template std::chrono::seconds::rep ecstasy::app::getLastAnimationTime<std::chrono::seconds>() const noexcept;

void printUsage(std::chrono::steady_clock::duration _duration) {}

void ecstasy::app::animate() {
    const auto current_timestamp = std::chrono::steady_clock::now();
    last_animation_time_ = current_timestamp - last_animation_start_timestamp_;
    last_animation_start_timestamp_ = current_timestamp;
    glfwPollEvents();

    const auto shouldBeginFrame = renderer_->beginFrame(filament_swapchain_);
    log::info("shouldBeginFrame: {}", shouldBeginFrame);

    if (shouldBeginFrame) {
        // log::info("{}us", getLastAnimationTime<std::chrono::microseconds>());
        scene_->animate(last_animation_time_);
        renderer_->endFrame();
        std::this_thread::sleep_for(5ms);
    }
}

ecstasy::app::~app() {
    scene_->destroy();
    filament_engine_->destroy(renderer_);
    filament_engine_->destroy(filament_swapchain_);
    filament_engine_->destroy(&filament_engine_);


    glfwDestroyWindow(window_);
    glfwTerminate();
}

/*
  ecstasy::scene ecstasy::app::createScene() {
    // registry_.emplace<>;
    std::cout << "Scene Created! " << std::endremovel;
}
*/
