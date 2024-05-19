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

/* bool loadTexture(filament::Engine* engine, const std::string& filePath, filament::Texture** map,
                 bool sRGB = true) {
    if (!filePath.empty()) {
        Path const path(filePath);
        if (path.exists()) {
            int w, h, n;
            unsigned char* data = stbi_load(path.getAbsolutePath().c_str(), &w, &h, &n, 3);
            if (data != nullptr) {
                *map = filament::Texture::Builder()
                           .width(uint32_t(w))
                           .height(uint32_t(h))
                           .levels(0xff)
                           .format(sRGB ? filament::Texture::InternalFormat::SRGB8
                                        : filament::Texture::InternalFormat::RGB8)
                           .build(*engine);
                filament::Texture::PixelBufferDescriptor buffer(
                    data, size_t(w * h * 3), filament::Texture::Format::RGB, filament::Texture::Type::UBYTE,
                    (filament::Texture::PixelBufferDescriptor::Callback)&stbi_image_free);
                (*map)->setImage(*engine, 0, std::move(buffer));
                (*map)->generateMipmaps(*engine);
                return true;
            } else {
                std::cout << "The texture " << path << " could not be loaded" << std::endl;
            }
        }
    }
    return false;
} */

class sandbox : public scene {
    app* app_;
    filament::Engine& filament_engine_;
    RendererResourceManager renderer_resource_manager_;
    filament::Renderer& renderer_;
    utils::Entity camera_entity_;
    filament::Camera* camera_;
    filament::View* view_;
    filament::Scene* scene_;
    InputController* input_controller_;
    EditorController* editor_controller_;

    skybox* skybox_;

    filament::VertexBuffer* vertex_buffer_;
    filament::IndexBuffer* index_buffer_;
    ecstasy::Material* material_;
    filament::MaterialInstance* material_instance_;

    utils::Entity light_;
    utils::Entity renderable_;

    box* north_wall_;
    box* south_wall_;
    box* east_wall_;
    box* west_wall_;

    Plane* plane_;

  public:
    sandbox() = delete;

    sandbox(filament::Engine& _filament_engine, filament::Renderer& _renderer,
            InputController* _input_controller)
        : filament_engine_{_filament_engine}, renderer_{_renderer},
          renderer_resource_manager_{RendererResourceManager(_filament_engine)} {
        auto viewport_dimension = _input_controller->getViewportDimension();

        scene_ = filament_engine_.createScene();
        view_ = filament_engine_.createView();
        view_->setPostProcessingEnabled(false);

        skybox_ = new skybox(filament_engine_, scene_);
        skybox_->buildClearColor();
        // skybox_->buildIBL();

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

        // editor_controller_ =
        //     new EditorController(_input_controller, camera_ /* , {0, 0, 50.0f}, {0, 0, 0} */);

        // north_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(north_wall_->getSolidRenderable());

        // view->setAmbientOcclusionOptions({.radius = 0.01f,
        //                                   .bilateralThreshold = 0.005f,
        //                                   .quality = View::QualityLevel::ULTRA,
        //                                   .lowPassFilter = View::QualityLevel::MEDIUM,
        //                                   .upsampling = View::QualityLevel::HIGH,
        //                                   .enabled = true});

        // material_ = ecstasy::shader::pbr(
        //     filament_engine_, "Wall Mat 1",
        //     shader::PBROptions{/* .baseColorMap = Eigen::Vector3d{193 / 255., 154 / 255., 107 / 255.} */});
        //
        // material_instance_ = material_->createInstance();
        //
        // int w, h, n;
        // auto path = std::filesystem::path("./xepkaecs_2K_Albedo.jpg").lexically_normal();
        // unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 3);
        // log::info("{} {} {}", w, h, n);
        // if (data != nullptr) {
        //     filament::Texture* map = filament::Texture::Builder()
        //                                  .width(uint32_t(w))
        //                                  .height(uint32_t(h))
        //                                  .levels(0xff)
        //                                  .format(filament::Texture::InternalFormat::SRGB8)
        //                                  .build(filament_engine_);
        //     filament::Texture::PixelBufferDescriptor buffer(
        //         data, size_t(w * h * 3), filament::Texture::Format::RGB, filament::Texture::Type::UBYTE,
        //         (filament::Texture::PixelBufferDescriptor::Callback)&stbi_image_free);
        //     map->setImage(filament_engine_, 0, std::move(buffer));
        //     // map->generateMipmaps(*filament_engine_);
        //
        //     filament::TextureSampler sampler(filament::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
        //                                      filament::TextureSampler::MagFilter::LINEAR,
        //                                      filament::TextureSampler::WrapMode::REPEAT);
        //     sampler.setAnisotropy(8.0f);
        //
        //     material_instance_->setParameter("baseColorMap", map, sampler);
        // } else {
        //     std::cout << "The texture " << path << " could not be loaded" << std::endl;
        // }

        material_ = new Material(filament_engine_, renderer_resource_manager_,
                                 Material::LitOptions{.albedo = "./xepkaecs_2K_Albedo.jpg",
                                                      .ao = "./xepkaecs_2K_AO.jpg",
                                                      .normalMap = "./xepkaecs_2K_Normal.jpg",
                                                      .roughness = "./xepkaecs_2K_Roughness.jpg"});

        /*   plane_ = new Plane(filament_engine_, renderer_resource_manager_,
                             Plane::GeometryOptions{.dimention = {10, 2.0, 0.2}, .segments = {1., 1., 1.}},
                             material_); */

        // plane_ = new plane(filament_engine_, {10, 2.0, 0.2}, material_, {1., 1., 1.},
        //                    Eigen::Vector3f{0., 0., 0.}, degreeToRad(Eigen::Vector3f{0., 0, 0.}),false);

        // scene_->addEntity(plane_->getRenderable());

        // south_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(south_wall_->getSolidRenderable());

        // east_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(east_wall_->getSolidRenderable());

        // west_wall_ = new box(*filament_engine_, {10., 2., .2}, material_, {1., 1., 1.}, false);
        // scene_->addEntity(west_wall_->getSolidRenderable());
        // scene_->addEntity(renderable_);

        // material->createInstance("tty");
    }

    void build() {}
    void animate(std::chrono::steady_clock::duration _last_animation_time) {

        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();

        // log::trace("Animation Time:", _last_animation_time.count());

        //        editor_controller_->animate(_last_animation_time);
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
        delete north_wall_;
    }
};
} // namespace scene

} // namespace ecstasy