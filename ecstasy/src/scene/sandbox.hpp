#pragma once
#include <common/common.hpp>
#include <scene/scene.hpp>
#include <ecstasy/ecstasy.hpp>
#include <controller/InputController.hpp>
#include <controller/EditorController.hpp>
#include <memory>

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
#include <numbers>

namespace ecstasy {
namespace scene {

struct Vertex {
    filament::math::float2 position;
    uint32_t color;
};

static const Vertex TRIANGLE_VERTICES[3] = {
    {{1, 0}, 0xffff0000u},
    {{cos(std::numbers::pi * 2 / 3), sin(std::numbers::pi * 2 / 3)}, 0xff00ff00u},
    {{cos(std::numbers::pi * 4 / 3), sin(std::numbers::pi * 4 / 3)}, 0xff0000ffu},
};

static constexpr uint16_t TRIANGLE_INDICES[3] = {0, 1, 2};

class sandbox : public scene {
    app* app_;
    filament::Engine* filament_engine_;
    filament::Renderer* renderer_;
    utils::Entity camera_entity_;
    filament::Camera* camera_;
    filament::View* view_;
    filament::Scene* scene_;
    filament::Skybox* skybox_;
    InputController* input_controller_;
    EditorController* editor_controller_;

  public:
    sandbox() = delete;

    sandbox(filament::Engine* _filament_engine, filament::Renderer* _renderer,
            InputController* _input_controller) {
        filament_engine_ = _filament_engine;
        renderer_ = _renderer;
        auto viewport_dimension = _input_controller->viewport_dimension_;

        auto cameraEntity = utils::EntityManager::get().create();
        camera_ = filament_engine_->createCamera(cameraEntity);
        view_ = filament_engine_->createView();
        scene_ = filament_engine_->createScene();

        view_->setCamera(camera_);
        view_->setScene(scene_);
        view_->setViewport({0, 0, static_cast<std::uint32_t>(viewport_dimension.x()),
                            static_cast<std::uint32_t>(viewport_dimension.y())});

        camera_->lookAt(filament::math::float3(0, 0, 50.f), filament::math::float3(0, 0, 0),
                        filament::math::float3(0, 1.f, 0));
        camera_->setProjection(
            90.0, static_cast<double>(viewport_dimension.x()) / static_cast<double>(viewport_dimension.y()),
            0.1, 50, filament::Camera::Fov::VERTICAL);

        editor_controller_ = new EditorController(_input_controller, camera_);

        skybox_ = filament::Skybox::Builder().color({0.1, 0.125, 0.25, 1.0}).build(*filament_engine_);
        scene_->setSkybox(skybox_);
        view_->setPostProcessingEnabled(false);

        static_assert(sizeof(Vertex) == 12, "Strange vertex size.");
        auto vb = filament::VertexBuffer::Builder()
                      .vertexCount(3)
                      .bufferCount(1)
                      .attribute(filament::VertexAttribute::POSITION, 0,
                                 filament::VertexBuffer::AttributeType::FLOAT2, 0, 12)
                      .attribute(filament::VertexAttribute::COLOR, 0,
                                 filament::VertexBuffer::AttributeType::UBYTE4, 8, 12)
                      .normalized(filament::VertexAttribute::COLOR)
                      .build(*filament_engine_);
        vb->setBufferAt(*filament_engine_, 0,
                        filament::VertexBuffer::BufferDescriptor(TRIANGLE_VERTICES, 36, nullptr));

        auto ib = filament::IndexBuffer::Builder()
                      .indexCount(3)
                      .bufferType(filament::IndexBuffer::IndexType::USHORT)
                      .build(*filament_engine_);
        ib->setBuffer(*filament_engine_,
                      filament::IndexBuffer::BufferDescriptor(TRIANGLE_INDICES, 6, nullptr));

        filamat::MaterialBuilder::init();
        filamat::MaterialBuilder builder;
        ecstasy::shader::simple(builder);
        filamat::Package package = builder.build(filament_engine_->getJobSystem());

        filament::Material* material = filament::Material::Builder()
                                           .package(package.getData(), package.getSize())
                                           .build(*filament_engine_);
        material->setDefaultParameter("baseColor", filament::RgbType::LINEAR,
                                      filament::math::float3{0, 1, 0});
        material->setDefaultParameter("metallic", 0.0f);
        material->setDefaultParameter("roughness", 0.4f);
        material->setDefaultParameter("reflectance", 0.5f);

        filament::MaterialInstance* materialInstance = material->createInstance();

        auto renderable = utils::EntityManager::get().create();

        utils::Entity light = utils::EntityManager::get().create();

        filament::LightManager::Builder(filament::LightManager::Type::SUN)
            .color(filament::Color::toLinear<filament::ACCURATE>(filament::sRGBColor(0.98f, 0.92f, 0.89f)))
            .intensity(150'000)
            //.direction({0.7, -1, -0.8})
            .direction({0, 0, 5})
            .sunAngularRadius(1.9f)
            .castShadows(true)
            .build(*filament_engine_, light);
        scene_->addEntity(light);

        filament::RenderableManager::Builder(1)
            .boundingBox({{-1, -1, -1}, {1, 1, 1}})
            .material(0, materialInstance)
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0, 3)
            .culling(false)
            .receiveShadows(false)
            .castShadows(false)
            .build(*filament_engine_, renderable);
    }

    void build() {}
    void animate(std::chrono::steady_clock::duration _last_animation_time) {
        editor_controller_->animate(_last_animation_time);
        renderer_->render(view_);
    }

    void destroy() {}
};
} // namespace scene

} // namespace ecstasy