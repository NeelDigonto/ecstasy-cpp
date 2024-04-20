#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filamat/MaterialBuilder.h>
#include <filament/Texture.h>
#include "stb_image.h"

#include <material/MaterialManager.hpp>

namespace ecstasy {
MaterialManager::MaterialManager(filament::Engine& _filament_engine) : filament_engine_{_filament_engine} {}
filament::Material* MaterialManager::getMaterial(filamat::MaterialBuilder& _builder,
                                                 const Material::Options& _options) {
    auto material_it = material_cache_.find(_options);

    if (material_it != std::end(material_cache_)) {
        return material_it->second;
    }

    filamat::Package package = _builder.build(filament_engine_.getJobSystem());

    auto material =
        filament::Material::Builder().package(package.getData(), package.getSize()).build(filament_engine_);

    material_cache_.insert({_options, material});
    return material;
}

filament::Texture* MaterialManager::getTexture(const std::string& _file_path, const bool sRGB) {
    auto texture_it = texture_cache_.find(_file_path);

    if (texture_it != std::end(texture_cache_)) {
        return texture_it->second;
    }

    int w, h, n;
    auto path = std::filesystem::path(_file_path).lexically_normal();
    stbi_info(path.c_str(), &w, &h, &n);
    log::info("Loading Texture ({}) with h:{}, w:{}, n:{}", path.c_str(), h, w, n);

    unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 3);
    if (!data) {
        const auto error_msg =
            fmt::format("Loading failed for texture ({}) with h:{}, w:{}, n:{}", path.c_str(), h, w, n);
        log::error(error_msg);
        throw new std::runtime_error(error_msg);
    }

    log::info("Loaded texture ({}) with h:{}, w:{}, n:{}", path.c_str(), h, w, n);

    filament::Texture* texture =
        filament::Texture::Builder()
            .width(uint32_t(w))
            .height(uint32_t(h))
            .levels(0xff)
            .format(sRGB ? filament::Texture::InternalFormat::SRGB8 : filament::Texture::InternalFormat::RGB8)
            .build(filament_engine_);

    filament::Texture::PixelBufferDescriptor buffer(
        data, size_t(w * h * 3), filament::Texture::Format::RGB, filament::Texture::Type::UBYTE,
        (filament::Texture::PixelBufferDescriptor::Callback)&stbi_image_free);

    texture->setImage(filament_engine_, 0, std::move(buffer));
    // texture->generateMipmaps(filament_engine_);

    texture_cache_.insert({_file_path, texture});
    return texture;
}
} // namespace ecstasy