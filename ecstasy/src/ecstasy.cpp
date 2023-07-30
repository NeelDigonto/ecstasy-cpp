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
#include <math/norm.h>
#include <shader/simple.hpp>

#include <fmt/core.h>

// GLFW_EXPOSE_NATIVE_WAYLAND GLFW_EXPOSE_NATIVE_X11
// #define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
// #define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>

Eigen::IOFormat ecstasy::CommaInitFmt(Eigen::StreamPrecision,
                                      Eigen::DontAlignCols, ", ", ", ", "", "",
                                      " << ", ";");
Eigen::IOFormat ecstasy::CleanFmt(4, 0, ", ", "\n", "[", "]");
Eigen::IOFormat ecstasy::OctaveFmt(Eigen::StreamPrecision, 0, ", ", ";\n", "",
                                   "", "[", "]");
Eigen::IOFormat ecstasy::HeavyFmt(Eigen::FullPrecision, 0, ", ", ";\n", "[",
                                  "]", "[", "]");

const static uint32_t indices[] = {0, 1, 2, 2, 3, 0};

const static filament::math::float3 vertices[] = {
    {-10, -10, 0},
    {-10, 10, 0},
    {10, 10, 0},
    {10, -10, 0},
};

/* filament::math::short4 tbn = filament::math::packSnorm16(
    filament::math::mat3f::packTangentFrame(
        filament::math::mat3f{filament::math::float3{1.0f, 0.0f, 0.0f},
                              filament::math::float3{0.0f, 0.0f, 1.0f},
                              filament::math::float3{0.0f, 1.0f, 0.0f}})
        .xyzw);

const static filament::math::short4 normals[]{tbn, tbn, tbn, tbn}; */

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
    glfwSetWindowUserPointer(window_, this);

    input_controller_ =
        new InputController(window_, {window_width_, window_height_});

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width,
                                               int height) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->updateViewportDimension({width, height});
    });

    filament_engine_ = filament::Engine::create(
        filament::Engine::Backend::OPENGL); // Engine::Backend::VULKAN

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

    camera_->lookAt(filament::math::float3(0, 0, 50.f),
                    filament::math::float3(0, 0, 0),
                    filament::math::float3(0, 1.f, 0));
    camera_->setProjection(90.0,
                           double(this->window_width_) / this->window_height_,
                           0.1, 50, filament::Camera::Fov::VERTICAL);

    editor_controller_ = new EditorController(input_controller_, camera_);

    skybox_ = filament::Skybox::Builder()
                  .color({0.1, 0.125, 0.25, 1.0})
                  .build(*filament_engine_);
    scene_->setSkybox(skybox_);
    view_->setPostProcessingEnabled(false);

    filament::VertexBuffer* vertexBuffer =
        filament::VertexBuffer::Builder()
            .vertexCount(4)
            .bufferCount(1)
            .attribute(filament::VertexAttribute::POSITION, 0,
                       filament::VertexBuffer::AttributeType::FLOAT3)
            /*             .attribute(filament::VertexAttribute::TANGENTS,
               1, filament::VertexBuffer::AttributeType::SHORT4)
                        .normalized(filament::VertexAttribute::TANGENTS) */
            .build(*filament_engine_);

    vertexBuffer->setBufferAt(
        *filament_engine_, 0,
        filament::VertexBuffer::BufferDescriptor(
            vertices, vertexBuffer->getVertexCount() * sizeof(vertices[0])));
    /*     vertexBuffer->setBufferAt(
            *filament_engine_, 1,
            filament::VertexBuffer::BufferDescriptor(
                normals, vertexBuffer->getVertexCount() * sizeof(normals[0])));
     */

    filament::IndexBuffer* indexBuffer =
        filament::IndexBuffer::Builder().indexCount(6).build(*filament_engine_);

    indexBuffer->setBuffer(
        *filament_engine_,
        filament::IndexBuffer::BufferDescriptor(
            indices, indexBuffer->getIndexCount() * sizeof(uint32_t)));

    filamat::MaterialBuilder::init();
    filamat::MaterialBuilder builder;
    ecstasy::shader::simple(builder);
    filamat::Package package = builder.build(filament_engine_->getJobSystem());

    filament::Material* material =
        filament::Material::Builder()
            .package(package.getData(), package.getSize())
            .build(*filament_engine_);
    material->setDefaultParameter("baseColor", filament::RgbType::LINEAR,
                                  filament::math::float3{0, 1, 0});
    material->setDefaultParameter("metallic", 0.0f);
    material->setDefaultParameter("roughness", 0.4f);
    material->setDefaultParameter("reflectance", 0.5f);

    filament::MaterialInstance* materialInstance = material->createInstance();

    utils::Entity renderable = utils::EntityManager::get().create();

    utils::Entity light = utils::EntityManager::get().create();

    filament::LightManager::Builder(filament::LightManager::Type::SUN)
        .color(filament::Color::toLinear<filament::ACCURATE>(
            filament::sRGBColor(0.98f, 0.92f, 0.89f)))
        .intensity(150'000)
        //.direction({0.7, -1, -0.8})
        .direction({0, 0, 5})
        .sunAngularRadius(1.9f)
        .castShadows(true)
        .build(*filament_engine_, light);

    scene_->addEntity(light);

    // build a quad
    filament::RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, materialInstance)
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES,
                  vertexBuffer, indexBuffer, 0, 6)
        .culling(false)
        .build(*filament_engine_, renderable);
    scene_->addEntity(renderable);
}

void ecstasy::app::setClearColor(const Eigen::Vector4d& _clear_color) noexcept {
    clear_color_ = _clear_color;
};

bool ecstasy::app::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
}

ecstasy::InputController* ecstasy::app::getInputController() noexcept {
    return input_controller_;
}

ecstasy::EditorController* ecstasy::app::getEditorController() noexcept {
    return editor_controller_;
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

void printUsage(std::chrono::steady_clock::duration _duration) {}

void ecstasy::app::animate() {
    const auto current_timestamp = std::chrono::steady_clock::now();
    last_animation_time_ = current_timestamp - last_animation_start_timestamp_;
    last_animation_start_timestamp_ = current_timestamp;

    glfwPollEvents();
    editor_controller_->animate(last_animation_time_);

    /*    constexpr double ZOOM = 1.5f;
       const uint32_t w = view_->getViewport().width;
       const uint32_t h = view_->getViewport().height;
       const double aspect = (double)w / h;

       camera_->setProjection(filament::Camera::Projection::PERSPECTIVE,
                              -aspect * ZOOM, aspect * ZOOM, -ZOOM, ZOOM,
       0.01, 10); */

    // camera_->setModelMatrix(filament::math::mat4f());

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
    std::cout << "Scene Created! " << std::endremovel;
}
*/
