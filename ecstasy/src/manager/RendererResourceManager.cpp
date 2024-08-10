#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filamat/MaterialBuilder.h>
#include <filament/Texture.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>
#include "stb_image.h"
#include <common/utils.hpp>
#include <fmt/format.h>

#include <filesystem>
#include <manager/RendererResourceManager.hpp>

namespace ecstasy {
RendererResourceManager::RendererResourceManager(filament::Engine& _filament_engine)
    : filament_engine_{_filament_engine} {}
filament::Material* RendererResourceManager::getMaterial(filamat::MaterialBuilder& _builder,
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

filament::Texture* RendererResourceManager::getTexture(const std::string& _file_path, const bool sRGB) {
    auto texture_it = texture_cache_.find(_file_path);

    if (texture_it != std::end(texture_cache_)) {
        return texture_it->second;
    }

    int w, h, n;
    auto path = std::filesystem::path(_file_path).lexically_normal();
    stbi_info(path.string().c_str(), &w, &h, &n);
    log::info("Loading Texture ({}) with h:{}, w:{}, n:{}", path.string(), h, w, n);

    unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &n, 3);
    if (!data) {
        const auto error_msg =
            fmt::format("Loading failed for texture ({}) with h:{}, w:{}, n:{}", path.string(), h, w, n);
        log::error(error_msg);
        throw new std::runtime_error(error_msg);
    }

    log::info("Loaded texture ({}) with h:{}, w:{}, n:{}", path.string(), h, w, n);

    filament::Texture* texture =
        filament::Texture::Builder()
            .width(uint32_t(w))
            .height(uint32_t(h))
            .levels(0xff)
            .format(sRGB ? filament::Texture::InternalFormat::SRGB8 : filament::Texture::InternalFormat::RGB8)
            .build(filament_engine_);

    filament::Texture::PixelBufferDescriptor buffer(
        data, size_t(w * h * 3), filament::Texture::Format::RGB, filament::Texture::Type::UBYTE
        /* ,(filament::Texture::PixelBufferDescriptor::Callback)&stbi_image_free */);

    texture->setImage(filament_engine_, 0, std::move(buffer));
    // texture->generateMipmaps(filament_engine_);

    texture_cache_.insert({_file_path, texture});
    return texture;
}

std::vector<Eigen::Vector3f> vertices;
std::vector<Eigen::Vector4f> normals;
std::vector<Eigen::Vector2f> uvs;
std::vector<std::uint32_t> indices;

void RendererResourceManager::deleteOldBuffer(void* _buffer) { pending_delete_buffers_.erase(_buffer); }

/* void RendererResourceManager::removeBufferFunction(void* _buffer, size_t _size,
                                                   void* _renderer_resource_manager) {
    auto renderer_resource_manager = (RendererResourceManager*)_renderer_resource_manager;
    renderer_resource_manager->deleteOldBuffer(_buffer);
} */

ecstasy::Plane::GeometryData
RendererResourceManager::getPlaneGeometryData(const ecstasy::Plane::GeometryOptions& _options) {
    auto place_geometry_data_it = plane_geometry_data_cache_.find(_options);

    // if (place_geometry_data_it != std::end(plane_geometry_data_cache_)) {
    //     return place_geometry_data_it->second;
    // }

    // static constexpr size_t WIREFRAME_OFFSET = 3 * 2 * 6;
    const Eigen::Vector2f half_dim = _options.dimention / 2.0;
    auto& hd = half_dim;

    vertices = {
        {-hd.x(), hd.y(), 0},  // top left
        {-hd.x(), -hd.y(), 0}, // bottom left
        {hd.x(), -hd.y(), 0},  // bottom right
        {hd.x(), hd.y(), 0},   // top right
    };

    normals = {
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
    };
    uvs = {
        Eigen::Vector2f{0, 1}, // 0. top left
        Eigen::Vector2f{0, 0}, // 1. bottom left
        Eigen::Vector2f{1, 0}, // 2. bottom right
        Eigen::Vector2f{1, 1}, // 3. top right
    };
    indices = {
        0, 1, 3, // bottom left triangle
        1, 2, 3  // top right triangle
    };
    /*     log::info("vectices {}", (void*)vertices_unmanaged.data());

        auto& vertices = std::get<std::vector<Eigen::Vector3f>>(
            pending_delete_buffers_.emplace(vertices_unmanaged.data(), std::move(vertices_unmanaged))
                .first->second);
        auto& normals = std::get<std::vector<Eigen::Vector4f>>(
            pending_delete_buffers_.emplace(normals_unmanaged.data(), std::move(normals_unmanaged))
                .first->second);
        auto& uvs = std::get<std::vector<Eigen::Vector2f>>(
            pending_delete_buffers_.emplace(uvs_unmanaged.data(), std::move(uvs_unmanaged)).first->second);
        auto& indices = std::get<std::vector<std::uint32_t>>(
            pending_delete_buffers_.emplace(indices_unmanaged.data(), std::move(indices_unmanaged))
                .first->second);

        log::info("vectices {}", (void*)vertices.data()); */

    filament::VertexBuffer* vertex_buffer =
        filament::VertexBuffer::Builder()
            .vertexCount(vertices.size())
            .bufferCount(3)
            .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT3)
            .attribute(filament::VertexAttribute::TANGENTS, 1, filament::VertexBuffer::AttributeType::FLOAT4)
            .normalized(filament::VertexAttribute::TANGENTS)
            .attribute(filament::VertexAttribute::UV0, 2, filament::VertexBuffer::AttributeType::FLOAT2)
            .normalized(filament::VertexAttribute::UV0)
            .build(filament_engine_);

    vertex_buffer->setBufferAt(
        filament_engine_, 0,
        filament::VertexBuffer::BufferDescriptor(vertices.data(),
                                                 vertex_buffer->getVertexCount() * sizeof(Eigen::Vector3f)));

    vertex_buffer->setBufferAt(
        filament_engine_, 1,
        filament::VertexBuffer::BufferDescriptor(normals.data(),
                                                 vertex_buffer->getVertexCount() * sizeof(Eigen::Vector4f)));

    vertex_buffer->setBufferAt(filament_engine_, 2,
                               filament::VertexBuffer::BufferDescriptor(
                                   uvs.data(), vertex_buffer->getVertexCount() * sizeof(Eigen::Vector2f)));

    filament::IndexBuffer* index_buffer =
        filament::IndexBuffer::Builder().indexCount(indices.size()).build(filament_engine_);

    index_buffer->setBuffer(filament_engine_,
                            filament::IndexBuffer::BufferDescriptor(
                                indices.data(), index_buffer->getIndexCount() * sizeof(uint32_t)));

    Plane::GeometryData plane_buffer_data{vertex_buffer, index_buffer};

    plane_geometry_data_cache_.insert({_options, plane_buffer_data});
    return plane_buffer_data;

    // Delete Vertex Buffer then Index Buffer
}

} // namespace ecstasy