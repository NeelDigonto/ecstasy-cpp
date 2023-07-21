#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>
#include <vector>

#include <ecstasy/ecstasy.hpp>
#include <Eigen/Dense>
#include <chrono>
#include <thread>

#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <filament/Options.h>
#include <filament/Box.h>
#include <filament/Camera.h>
#include <filament/Color.h>
#include <filament/VertexBuffer.h>
#include <filament/Fence.h>
#include <filament/IndexBuffer.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filamat/MaterialBuilder.h>
#include <filament/LightManager.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/Stream.h>
#include <filament/SwapChain.h>
#include <filament/View.h>
#include <filament/Texture.h>
#include <filament/Skybox.h>
#include <filament/TextureSampler.h>
#include <filament/Viewport.h>
#include <filament/IndirectLight.h>

#include <utils/Entity.h>
#include <utils/EntityManager.h>

// GLFW_EXPOSE_NATIVE_WAYLAND GLFW_EXPOSE_NATIVE_X11
// #define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
// #define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>

// using utils::Entity;
using utils::EntityManager;

struct App {
    filament::Engine* engine;
    filament::SwapChain* swapChain;
    filament::Renderer* renderer;
    filament::View* view;
    filament::Scene* scene;
    filament::Camera* cam;

    filament::VertexBuffer* vb;
    filament::IndexBuffer* ib;
    filament::Material* mat;
    filament::Skybox* skybox;

    // Entity camera;
    // Entity renderable;
};

App app;

struct Vertex {
    filament::math::float2 position;
    uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
    {{1, 0}, 0xffff0000u},
    {{cos(M_PI * 2 / 3), sin(M_PI * 2 / 3)}, 0xff00ff00u},
    {{cos(M_PI * 4 / 3), sin(M_PI * 4 / 3)}, 0xff0000ffu},
};

static constexpr uint16_t TRIANGLE_INDICES[3] = {0, 1, 2};

// https : // github.com/BinomialLLC/basis_universal
ecstasy::app::app(std::string _app_name, std::uint32_t _window_width,
                  std::uint32_t _window_height) {
    app_name_ = _app_name;
    window_height_ = _window_height;
    window_width_ = _window_width;

    if (!glfwInit())
        throw std::runtime_error("Could not initialize GLFW!");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(window_width_, window_height_, app_name_.c_str(),
                               NULL, NULL);
    if (!window_)
        throw std::runtime_error("Could not open window!");

    glfwMakeContextCurrent(window_);

    filament_engine_ = filament::Engine::create(
        filament::Engine::Backend::OPENGL); // Engine::Backend::VULKAN

    // glfwGetWaylandWindow(window_);
    auto native_window = glfwGetX11Window(window_);
    filament_swapchain_ =
        filament_engine_->createSwapChain((void*)native_window);
    renderer_ = filament_engine_->createRenderer();

    auto cameraEntity = utils::EntityManager::get().create();
    camera_ = filament_engine_->createCamera(cameraEntity);
    view_ = filament_engine_->createView();
    scene_ = filament_engine_->createScene();

    view_->setCamera(camera_);
    view_->setScene(scene_);
    view_->setViewport({0, 0, window_width_, window_height_});

    skybox_ = filament::Skybox::Builder()
                  .color({0.1, 0.125, 0.25, 1.0})
                  .build(*filament_engine_);
    scene_->setSkybox(skybox_);
    view_->setPostProcessingEnabled(false);
}

void ecstasy::app::setClearColor(const Eigen::Vector4d& _clear_color) noexcept {
    clear_color_ = _clear_color;
};

bool ecstasy::app::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
}

template <ecstasy::AnimationTime T>
typename T::rep ecstasy::app::getLastAnimationTime() const noexcept {
    return std::chrono::duration_cast<T>(last_animation_time_).count();
}
template std::chrono::nanoseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::nanoseconds>() const noexcept;
template std::chrono::microseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::microseconds>() const noexcept;
template std::chrono::milliseconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::milliseconds>() const noexcept;
template std::chrono::seconds::rep
ecstasy::app::getLastAnimationTime<std::chrono::seconds>() const noexcept;

void ecstasy::app::animate() {
    const auto current_timestamp = std::chrono::steady_clock::now();
    last_animation_time_ = current_timestamp - last_animation_start_timestamp_;
    last_animation_start_timestamp_ = current_timestamp;

    glfwPollEvents();

    constexpr double ZOOM = 1.5f;
    const uint32_t w = view_->getViewport().width;
    const uint32_t h = view_->getViewport().height;
    const double aspect = (double)w / h;

    camera_->setProjection(filament::Camera::Projection::ORTHO, -aspect * ZOOM,
                           aspect * ZOOM, -ZOOM, ZOOM, 0, 1);

    if (renderer_->beginFrame(filament_swapchain_)) {
        renderer_->render(view_);
        renderer_->endFrame();
    }
}

ecstasy::app::~app() {
    filament_engine_->destroy(&filament_engine_);

    glfwDestroyWindow(window_);
    glfwTerminate();
}

/*
  ecstasy::scene ecstasy::app::createScene() {
    // registry_.emplace<>;
    std::cout << "Scene Created! " << std::endl;
}
*/