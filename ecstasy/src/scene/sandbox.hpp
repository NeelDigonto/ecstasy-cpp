#pragma once
#include <common/common.hpp>
#include <scene/scene.hpp>
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

namespace ecstasy {
namespace scene {
class sandbox : public scene {
    /*     app* app_;

        utils::Entity camera_entity_;
        filament::Camera* camera_;
        filament::View* view_;
        filament::Scene* scene_;
        filament::Skybox* skybox_;
        EditorController* editor_controller_;

        sandbox() = delete;

        sandbox(filament::Engine* _filament_engine, InputController* _input_controller) {
            camera_entity_ = utils::EntityManager::get().create();
            camera_ = filament_engine_->createCamera(camera_entity_);
            view_ = filament_engine_->createView();
            scene_ = filament_engine_->createScene();

            view_->setCamera(camera_);
            view_->setScene(scene_);
            view_->setViewport({0, 0, window_width_, window_height_});

            camera_->lookAt(filament::math::float3(0, 0, 50.f), filament::math::float3(0, 0, 0),
                            filament::math::float3(0, 1.f, 0));
            camera_->setProjection(90.0, double(this->window_width_) / this->window_height_, 0.1, 50,
                                   filament::Camera::Fov::VERTICAL);

            skybox_ = filament::Skybox::Builder().color({0.1, 0.125, 0.25, 1.0}).build(*_filament_engine);
            scene_->setSkybox(skybox_);
            view_->setPostProcessingEnabled(false);
        } */

    void build() {}
    void animate() {
        // editor_controller_->animate(last_animation_time_);
        // renderer_->render(view_);
    }

    void destroy() {}
};
} // namespace scene

} // namespace ecstasy