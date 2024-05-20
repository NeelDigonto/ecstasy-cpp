#pragma once
#include <filesystem>
#include <filament/Texture.h>
#include <filament/Skybox.h>
#include "stb_image.h"

namespace ecstasy {
class skybox {
  private:
    filament::Engine& filament_engine_;
    filament::Scene& scene_;
    filament::Skybox* skybox_;
    filament::Texture* skybox_texture_;
    filament::Texture* ibl_texture_;
    filament::Texture* fog_texture_;
    filament::IndirectLight* indirect_light_;

  public:
    skybox(filament::Engine& _filament_engine, filament::Scene& _scene)
        : filament_engine_{_filament_engine}, scene_{_scene} {}

    void buildClearColor(Eigen::Vector4d _color = {0.1, 0.125, 0.25, 1.0}) {
        skybox_ = filament::Skybox::Builder()
                      .color({_color.x(), _color.y(), _color.z(), _color.w()})
                      .build(filament_engine_);

        scene_.setSkybox(skybox_);
    }

    void buildIBL() {
        // ------------------------------------------------------------------
        // ------------- LOAD SKYBOX ----------------------------------------
        // ------------------------------------------------------------------
        auto path = std::filesystem::path("./dancing_hall_8k.hdr").lexically_normal();

        int w, h;
        stbi_info(path.string().c_str(), &w, &h, nullptr);
        log::info("{}, {}", w, h);
        if (w != h * 2) {
            log::error("not an equirectangular image!");
        }

        // load image as float
        int n;
        const size_t size = w * h * sizeof(filament::math::float3);
        filament::math::float3* const data =
            (filament::math::float3*)stbi_loadf(path.string().c_str(), &w, &h, &n, 3);
        if (data == nullptr || n != 3) {
            log::error("Could not decode image ");
        }

        // now load texture
        filament::Texture::PixelBufferDescriptor buffer(
            data, size, filament::Texture::Format::RGB, filament::Texture::Type::FLOAT,
            [](void* buffer, size_t size, void* user) { stbi_image_free(buffer); });

        filament::Texture* const equirect = filament::Texture::Builder()
                                                .width((uint32_t)w)
                                                .height((uint32_t)h)
                                                .levels(0xff)
                                                .format(filament::Texture::InternalFormat::R11F_G11F_B10F)
                                                .sampler(filament::Texture::Sampler::SAMPLER_2D)
                                                .build(filament_engine_);

        equirect->setImage(filament_engine_, 0, std::move(buffer));

        IBLPrefilterContext context(filament_engine_);
        IBLPrefilterContext::EquirectangularToCubemap equirectangularToCubemap(context);
        IBLPrefilterContext::SpecularFilter specularFilter(context);
        IBLPrefilterContext::IrradianceFilter irradianceFilter(context);

        skybox_texture_ = equirectangularToCubemap(equirect);
        filament_engine_.destroy(equirect);

        ibl_texture_ = specularFilter(skybox_texture_);

        fog_texture_ = irradianceFilter({.generateMipmap = false}, skybox_texture_);
        fog_texture_->generateMipmaps(filament_engine_);

        indirect_light_ = filament::IndirectLight::Builder()
                              .reflections(ibl_texture_)
                              .intensity(60000.0f)
                              .build(filament_engine_);

        scene_.setIndirectLight(indirect_light_);

        skybox_ =
            filament::Skybox::Builder().environment(skybox_texture_).showSun(false).build(filament_engine_);

        scene_.setSkybox(skybox_);
    }

    ~skybox() { filament_engine_.destroy(skybox_); }
};
} // namespace ecstasy