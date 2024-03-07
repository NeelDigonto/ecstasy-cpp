#include <geometry/plane.hpp>
#include <common/utils.hpp>
#include <filament/Material.h>

#include <utils/EntityManager.h>

#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>

ecstasy::plane::plane(filament::Engine& _filament_engine, Eigen::Vector3d _dimention,
                      filament::Material const* _material, Eigen::Vector3d _linear_color, bool _culling)
    : filament_engine_(_filament_engine), material_(_material) {
    const Eigen::Vector3d half_dim = _dimention / 2.0;
    auto& hd = half_dim;

    vertices_ = {
        {-hd.x(), hd.y(), 0},  // top left
        {-hd.x(), -hd.y(), 0}, // bottom left
        {hd.x(), -hd.y(), 0},  // bottom right
        {hd.x(), hd.y(), 0},   // top right
    };

    normals_ = {
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}), //
    };

    uvs_ = {
        Eigen::Vector2f{0, 1}, // 0. top left
        Eigen::Vector2f{0, 0}, // 1. bottom left
        Eigen::Vector2f{1, 0}, // 2. bottom right
        Eigen::Vector2f{1, 1}, // 3. top right
    };

    indices_ = {
        0, 1, 3, // bottom left triangle
        1, 2, 3  // top right triangle
    };

    vertex_buffer_ =
        filament::VertexBuffer::Builder()
            .vertexCount(vertices_.size())
            .bufferCount(3)
            .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT3)
            .attribute(filament::VertexAttribute::TANGENTS, 1, filament::VertexBuffer::AttributeType::FLOAT4)
            .normalized(filament::VertexAttribute::TANGENTS)
            .attribute(filament::VertexAttribute::UV0, 2, filament::VertexBuffer::AttributeType::FLOAT2)
            .normalized(filament::VertexAttribute::UV0)
            .build(filament_engine_);

    vertex_buffer_->setBufferAt(
        filament_engine_, 0,
        filament::VertexBuffer::BufferDescriptor(
            vertices_.data(), vertex_buffer_->getVertexCount() * sizeof(decltype(vertices_)::value_type)));

    vertex_buffer_->setBufferAt(
        filament_engine_, 1,
        filament::VertexBuffer::BufferDescriptor(
            normals_.data(), vertex_buffer_->getVertexCount() * sizeof(decltype(normals_)::value_type)));

    vertex_buffer_->setBufferAt(
        filament_engine_, 2,
        filament::VertexBuffer::BufferDescriptor(uvs_.data(), vertex_buffer_->getVertexCount() *
                                                                  sizeof(decltype(uvs_)::value_type)));

    index_buffer_ = filament::IndexBuffer::Builder().indexCount(indices_.size()).build(filament_engine_);

    index_buffer_->setBuffer(filament_engine_,
                             filament::IndexBuffer::BufferDescriptor(
                                 indices_.data(), index_buffer_->getIndexCount() * sizeof(uint32_t)));

    material_instance_ = material_->createInstance();

    utils::EntityManager& em = utils::EntityManager::get();
    renderable_ = em.create();
    filament::RenderableManager::Builder(1)
        .boundingBox({{-hd.x(), -hd.y(), -hd.z()}, {hd.x(), hd.y(), hd.z()}})
        .material(0, material_instance_)
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vertex_buffer_, index_buffer_, 0,
                  index_buffer_->getIndexCount())
        .priority(4)
        .culling(_culling)
        .build(filament_engine_, renderable_);

    filament::TransformManager& tm = filament_engine_.getTransformManager();
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> ecstasy::plane::getBoundingBox() { return {}; }

ecstasy::plane::~plane() {
    filament_engine_.destroy(vertex_buffer_);
    filament_engine_.destroy(index_buffer_);
    filament_engine_.destroy(material_instance_);
    // We don't own the material, only instances
    filament_engine_.destroy(renderable_);

    utils::EntityManager& em = utils::EntityManager::get();
    em.destroy(renderable_);
}