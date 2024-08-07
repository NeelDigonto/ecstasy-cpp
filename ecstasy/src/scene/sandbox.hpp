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
#include <geometry/Plane.hpp>
#include <geometry/Box.hpp>

#include <material/Material.hpp>
#include <manager/RendererResourceManager.hpp>

namespace ecstasy {

namespace scene {

class sandbox : public scene {
    filament::Engine& filament_engine_;
    RendererResourceManager& renderer_resource_manager_;
    filament::Renderer& renderer_;
    InputController& input_controller_;
    utils::Entity camera_entity_;
    filament::Camera* camera_;
    filament::View* view_;
    filament::Scene* scene_;
    EditorController* editor_controller_;

    skybox* skybox_;

    filament::VertexBuffer* vertex_buffer_;
    filament::IndexBuffer* index_buffer_;
    ecstasy::Material* material_;

    utils::Entity light_;
    utils::Entity renderable_;

    Plane* plane_;
    Box* box_;

  public:
    sandbox() = delete;

    sandbox(filament::Engine& _filament_engine, filament::Renderer& _renderer,
            RendererResourceManager& _renderer_resource_manager, InputController& _input_controller)
        : filament_engine_{_filament_engine}, renderer_{_renderer},
          renderer_resource_manager_{_renderer_resource_manager}, input_controller_{_input_controller} {
        auto viewport_dimension = input_controller_.getViewportDimension();

        scene_ = filament_engine_.createScene();
        view_ = filament_engine_.createView();
        view_->setPostProcessingEnabled(false);

        skybox_ = new skybox(filament_engine_, *scene_);
        // skybox_->buildClearColor();
        skybox_->buildIBL();

        light_ = utils::EntityManager::get().create();
        filament::LightManager::Builder(filament::LightManager::Type::SUN)
            .color(filament::Color::toLinear<filament::ACCURATE>(filament::sRGBColor(0.98f, 0.92f, 0.89f)))
            .intensity(200'000)
            .direction({20, 20, 20})
            .sunAngularRadius(1.9f)
            .castShadows(true)
            .build(filament_engine_, light_);
        scene_->addEntity(light_);

        camera_entity_ = utils::EntityManager::get().create();
        camera_ = filament_engine_.createCamera(camera_entity_);
        view_->setCamera(camera_);
        view_->setScene(scene_);
        view_->setViewport({0, 0, static_cast<std::uint32_t>(viewport_dimension.x()),
                            static_cast<std::uint32_t>(viewport_dimension.y())});

        editor_controller_ =
            new EditorController(input_controller_, *camera_ /* , {0, 0, 50.0f}, {0, 0, 0} */);

        material_ = new Material(filament_engine_, renderer_resource_manager_,
                                 Material::LitOptions{.albedo = "./xepkaecs_2K_Albedo.jpg",
                                                      .ao = "./xepkaecs_2K_AO.jpg",
                                                      .normalMap = "./xepkaecs_2K_Normal.jpg",
                                                      .roughness = "./xepkaecs_2K_Roughness.jpg"});

        /* plane_ = new Plane(filament_engine_, renderer_resource_manager_,
                           Plane::GeometryOptions{.dimention = {10, 2.0}, .segments = {1., 1.}}, material_);
         */

        // scene_->addEntity(plane_->getRenderable());
        // 10, 2.0, 0.2

        box_ = new Box(filament_engine_, renderer_resource_manager_,
                       Box::Options{
                           .dimention = Eigen::Vector3f{10.0, 2.0, 0.2},
                           .px_materials_ = material_,
                           .nx_materials_ = material_,
                           .py_materials_ = material_,
                           .ny_materials_ = material_,
                           .pz_materials_ = material_,
                           .nz_materials_ = material_,
                       });
        box_->addRenderablesToScene(*scene_);
    }

    void build() {}
    void animate(std::chrono::steady_clock::duration _last_animation_time) {

        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();

        // log::trace("Animation Time:", _last_animation_time.count());

        editor_controller_->animate(_last_animation_time);
        renderer_.render(view_);

        // Render UI

        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void destroy() {
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
        delete skybox_;
        delete material_;
        filament_engine_.destroy(view_);
        filament_engine_.destroyCameraComponent(camera_entity_);
        filament_engine_.destroy(light_);
        filament_engine_.destroy(scene_);
    }
};
} // namespace scene

} // namespace ecstasy

// view->setAmbientOcclusionOptions({.radius = 0.01f,
//                                   .bilateralThreshold = 0.005f,
//                                   .quality = View::QualityLevel::ULTRA,
//                                   .lowPassFilter = View::QualityLevel::MEDIUM,
//                                   .upsampling = View::QualityLevel::HIGH,
//                                   .enabled = true});