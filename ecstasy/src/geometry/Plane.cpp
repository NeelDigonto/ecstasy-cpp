#include <geometry/Plane.hpp>
#include <common/utils.hpp>
#include <filament/Material.h>

#include <utils/EntityManager.h>

#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <Eigen/Dense>

#include <material/Material.hpp>
#include <manager/RendererResourceManager.hpp>

ecstasy::Plane::Plane(filament::Engine& _filament_engine, RendererResourceManager& _renderer_resource_manager,
                      ecstasy::Plane::GeometryOptions _geometry_options, ecstasy::Material* _material)
    : filament_engine_(_filament_engine), renderer_resource_manager_{_renderer_resource_manager},
      material_(_material), Transformable(_filament_engine.getTransformManager()) {
    const Eigen::Vector3f half_dim = _geometry_options.dimention / 2.0;
    auto& hd = half_dim;

    geometry_data_ = renderer_resource_manager_.getPlaneGeometryData(_geometry_options);

    material_instance_ = material_->createInstance();

    renderable_ = getEntity();
    filament::RenderableManager::Builder(1)
        .boundingBox({{-hd.x(), -hd.y(), -hd.z()}, {hd.x(), hd.y(), hd.z()}})
        .material(0, material_instance_)
        .geometry(0, filament::RenderableManager::PrimitiveType::TRIANGLES, geometry_data_.vertex_buffer,
                  geometry_data_.index_buffer, 0, geometry_data_.index_buffer->getIndexCount())
        .priority(4)
        .build(filament_engine_, renderable_);

    // setRotation({0., degreeToRad(90.0f), 0.});
    setTranslation({10., 10., 10.});
    computeTransform();
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> ecstasy::Plane::getBoundingBox() { return {}; }

ecstasy::Plane::~Plane() {
    filament_engine_.destroy(material_instance_);
    // We don't own the material, only instances
    filament_engine_.destroy(renderable_);

    utils::EntityManager& em = utils::EntityManager::get();
    em.destroy(renderable_);
}