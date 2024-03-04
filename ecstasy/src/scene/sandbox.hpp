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
#include <filament-iblprefilter/IBLPrefilterContext.h>

#include <utils/Entity.h>
#include <utils/EntityManager.h>
#include <math/norm.h>
#include <math/mat3.h>
#include <numbers>

#include <common/imgui.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <fstream>
#include <filesystem>
#include <common/utils.hpp>

#include "stb_image.h"
#include <skybox/skybox.hpp>
#include <geometry/plane.hpp>
#include <geometry/box.hpp>

namespace ecstasy {

namespace scene {

const static std::vector<uint32_t> indices = {0, 1, 2};

const static std::vector<Eigen::Vector3f> vertices = {
    Eigen::Vector3f{-5, -5, 0},
    Eigen::Vector3f{0, 5, 0},
    Eigen::Vector3f{5, -5, 0},
};

const static std::vector<Eigen::Vector4f> normals{getFloat4FromEuler({0.0f, 0.0f, 1.0f}),
                                                  getFloat4FromEuler({0.0f, 0.0f, 1.0f}),
                                                  getFloat4FromEuler({0.0f, 0.0f, 1.0f})};

class sandbox : public scene {
    app* app_;
    filament::Engine* filament_engine_;
    filament::Renderer* renderer_;
    utils::Entity camera_entity_;
    filament::Camera* camera_;
    filament::View* view_;
    filament::Scene* scene_;
    InputController* input_controller_;
    EditorController* editor_controller_;
    // filament::Skybox* skybox_;
    // filament::Texture* skybox_texture_;
    // filament::Texture* ibl_texture_;
    // filament::Texture* fog_texture_;
    // filament::IndirectLight* indirect_light_;

    skybox* skybox_;

    filament::VertexBuffer* vertex_buffer_;
    filament::IndexBuffer* index_buffer_;
    filament::Material* material_;
    filament::MaterialInstance* material_instance_;

    utils::Entity light_;
    utils::Entity renderable_;

    box* north_wall_;
    box* south_wall_;
    box* east_wall_;
    box* west_wall_;

    plane* plane_;

  public:
    sandbox() = delete;

    sandbox(filament::Engine* _filament_engine, filament::Renderer* _renderer,
            InputController* _input_controller) {

        filament_engine_ = _filament_engine;
        renderer_ = _renderer;
        auto viewport_dimension = _input_controller->getViewportDimension();

        scene_ = filament_engine_->createScene();
        view_ = filament_engine_->createView();
        view_->setPostProcessingEnabled(false);

        skybox_ = new skybox(filament_engine_, scene_);
        // skybox_->buildClearColor();
        skybox_->buildIBL();

        light_ = utils::EntityManager::get().create();
        filament::LightManager::Builder(filament::LightManager::Type::SUN)
            .color(filament::Color::toLinear<filament::ACCURATE>(filament::sRGBColor(0.98f, 0.92f, 0.89f)))
            .intensity(200'000)
            .direction({20, 20, 20})
            .sunAngularRadius(1.9f)
            .castShadows(true)
            .build(*filament_engine_, light_);
        scene_->addEntity(light_);

        auto cameraEntity = utils::EntityManager::get().create();
        camera_ = filament_engine_->createCamera(cameraEntity);
        view_->setCamera(camera_);
        view_->setScene(scene_);
        view_->setViewport({0, 0, static_cast<std::uint32_t>(viewport_dimension.x()),
                            static_cast<std::uint32_t>(viewport_dimension.y())});

        editor_controller_ =
            new EditorController(_input_controller, camera_ /* , {0, 0, 50.0f}, {0, 0, 0} */);

        filamat::MaterialBuilder::init();
        filamat::MaterialBuilder builder;
        ecstasy::shader::simple(builder);
        filamat::Package package = builder.build(filament_engine_->getJobSystem());

        material_ = filament::Material::Builder()
                        .package(package.getData(), package.getSize())
                        .build(*filament_engine_);
        material_->setDefaultParameter("baseColor", filament::RgbType::LINEAR,
                                       filament::math::float3{0, 1, 0});
        material_->setDefaultParameter("metallic", 0.0f);
        material_->setDefaultParameter("roughness", 0.4f);
        material_->setDefaultParameter("reflectance", 0.5f);

        material_instance_ = material_->createInstance();

        vertex_buffer_ = filament::VertexBuffer::Builder()
                             .vertexCount(3)
                             .bufferCount(2)
                             .attribute(filament::VertexAttribute::POSITION, 0,
                                        filament::VertexBuffer::AttributeType::FLOAT3)
                             .attribute(filament::VertexAttribute::TANGENTS, 1,
                                        filament::VertexBuffer::AttributeType::FLOAT4)
                             .normalized(filament::VertexAttribute::TANGENTS)
                             .build(*filament_engine_);

        vertex_buffer_->setBufferAt(
            *filament_engine_, 0,
            filament::VertexBuffer::BufferDescriptor(vertices.data(),
                                                     vertex_buffer_->getVertexCount() * sizeof(vertices[0])));
        vertex_buffer_->setBufferAt(
            *filament_engine_, 1,
            filament::VertexBuffer::BufferDescriptor(normals.data(),
                                                     vertex_buffer_->getVertexCount() * sizeof(normals[0])));

        index_buffer_ = filament::IndexBuffer::Builder().indexCount(3).build(*filament_engine_);

        index_buffer_->setBuffer(*filament_engine_,
                                 filament::IndexBuffer::BufferDescriptor(
                                     indices.data(), index_buffer_->getIndexCount() * sizeof(uint32_t)));

        renderable_ = utils::EntityManager::get().create();

        filament::RenderableManager::Builder(1)
            .boundingBox({{-1, -1, -1}, {1, 1, 1}})
            .material(0, material_instance_)
            .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vertex_buffer_, index_buffer_,
                      0, 3)
            .culling(false)
            .receiveShadows(false)
            .castShadows(false)
            .build(*filament_engine_, renderable_);

        north_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(north_wall_->getSolidRenderable());

        plane_ = new plane(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        scene_->addEntity(plane_->getRenderable());

        // south_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(south_wall_->getSolidRenderable());

        // east_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(east_wall_->getSolidRenderable());

        // west_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(west_wall_->getSolidRenderable());
        // scene_->addEntity(renderable_);
    }

    void build() {}
    void animate(std::chrono::steady_clock::duration _last_animation_time) {

        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();

        editor_controller_->animate(_last_animation_time);
        renderer_->render(view_);

        // Render UI

        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void destroy() {
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
        delete skybox_;
        delete north_wall_;
    }
};
} // namespace scene

} // namespace ecstasy