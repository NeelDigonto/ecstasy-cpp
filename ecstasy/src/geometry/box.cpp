#include <geometry/box.hpp>
#include <common/utils.hpp>
#include <filament/Material.h>

#include <utils/EntityManager.h>

#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/TransformManager.h>

ecstasy::box::box(filament::Engine& _filament_engine, Eigen::Vector3d _dimention,
                  filament::Material const* _material, Eigen::Vector3d _linear_color, bool _culling)
    : filament_engine_(_filament_engine), material_(_material) {
    const Eigen::Vector3d half_dim = _dimention / 2.0;
    auto& hd = half_dim;

    vertices_ = {
        {-hd.x(), -hd.y(), hd.z()},  // 0. left bottom far
        {hd.x(), -hd.y(), hd.z()},   // 1. right bottom far
        {-hd.x(), hd.y(), hd.z()},   // 2. left top far
        {hd.x(), hd.y(), hd.z()},    // 3. right top far
        {-hd.x(), -hd.y(), -hd.z()}, // 4. left bottom near
        {hd.x(), -hd.y(), -hd.z()},  // 5. right bottom near
        {-hd.x(), hd.y(), -hd.z()},  // 6. left top near
        {hd.x(), hd.y(), -hd.z()},   // 7. right top near
    };

    vertices_ = {
        {hd.x(), -hd.y(), hd.z()}, // 0. px far bottom
        {hd.x(), -hd.y(), hd.z()}, // 0. px far top
        {hd.x(), -hd.y(), hd.z()}, // 0. px near bottom
        {hd.x(), -hd.y(), hd.z()}, // 0. px near top
    };

    normals_ = {
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), // 0. left bottom far
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), // 1. right bottom far
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), // 2. left top far
        getFloat4FromEuler({0.0f, 0.0f, -1.0f}), // 3. right top far
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  // 4. left bottom near
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  // 5. right bottom near
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  // 6. left top near
        getFloat4FromEuler({0.0f, 0.0f, 1.0f}),  // 7. right top near
    };

    uvs_ = {
        Eigen::Vector2f{0, 0}, // 0. left bottom far
        Eigen::Vector2f{1, 0}, // 1. right bottom far
        Eigen::Vector2f{0, 1}, // 2. left top far
        Eigen::Vector2f{1, 1}, // 3. right top far
        Eigen::Vector2f{0, 0}, // 4. left bottom near
        Eigen::Vector2f{1, 0}, // 5. right bottom near
        Eigen::Vector2f{0, 1}, // 6. left top near
        Eigen::Vector2f{1, 1}, // 7. right top near
    };

    indices_ = {
        2, 0, 1, 2, 1, 3, // far
        6, 4, 5, 6, 5, 7, // near
        2, 0, 4, 2, 4, 6, // left
        3, 1, 5, 3, 5, 7, // right
        0, 4, 5, 0, 5, 1, // bottom
        2, 6, 7, 2, 7, 3, // top

        // NOLINTBEGIN
        // wire-frame
        // 0, 1, 1, 3, 3, 2, 2, 0, // far
        // 4, 5, 5, 7, 7, 6, 6, 4, // near
        // 0, 4, 1, 5, 3, 7, 2, 6,
        // NOLINTEND
    };

    vertex_buffer_ =
        filament::VertexBuffer::Builder()
            .vertexCount(8)
            .bufferCount(2)
            .attribute(filament::VertexAttribute::POSITION, 0, filament::VertexBuffer::AttributeType::FLOAT3)
            .attribute(filament::VertexAttribute::TANGENTS, 1, filament::VertexBuffer::AttributeType::FLOAT4)
            .normalized(filament::VertexAttribute::TANGENTS)
            .build(filament_engine_);

    vertex_buffer_->setBufferAt(
        filament_engine_, 0,
        filament::VertexBuffer::BufferDescriptor(vertices_.data(),
                                                 vertex_buffer_->getVertexCount() * sizeof(vertices_[0])));

    vertex_buffer_->setBufferAt(filament_engine_, 1,
                                filament::VertexBuffer::BufferDescriptor(
                                    normals_.data(), vertex_buffer_->getVertexCount() * sizeof(normals_[0])));

    index_buffer_ = filament::IndexBuffer::Builder().indexCount(indices_.size()).build(filament_engine_);

    index_buffer_->setBuffer(filament_engine_,
                             filament::IndexBuffer::BufferDescriptor(
                                 indices_.data(), index_buffer_->getIndexCount() * sizeof(uint32_t)));

    if (material_) {
        material_instance_solid_ = material_->createInstance();
        // material_instance_wireframe_ = material_->createInstance();
        // material_instance_solid_->setParameter(
        //     "baseColor", filament::RgbaType::LINEAR,
        //     filament::LinearColorA{_linear_color.x(), _linear_color.y(), _linear_color.z(), 0.05f});
        // material_instance_wireframe_->setParameter(
        //     "baseColor", filament::RgbaType::LINEAR,
        //     filament::LinearColorA{_linear_color.x(), _linear_color.y(), _linear_color.z(), 0.25f});
    }

    utils::EntityManager& em = utils::EntityManager::get();
    solid_renderable_ = em.create();
    filament::RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, material_instance_solid_)
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, vertex_buffer_, index_buffer_, 0,
                  index_buffer_->getIndexCount())
        .priority(4)
        .culling(false)
        .build(filament_engine_, solid_renderable_);

    filament::TransformManager& tm = filament_engine_.getTransformManager();
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> ecstasy::box::getBoundingBox() { return {}; }

ecstasy::box::~box() {
    filament_engine_.destroy(vertex_buffer_);
    filament_engine_.destroy(index_buffer_);
    filament_engine_.destroy(material_instance_solid_);
    filament_engine_.destroy(material_instance_wireframe_);
    // We don't own the material, only instances
    filament_engine_.destroy(solid_renderable_);
    filament_engine_.destroy(wireframe_renderable_);

    utils::EntityManager& em = utils::EntityManager::get();
    em.destroy(solid_renderable_);
    em.destroy(wireframe_renderable_);
}