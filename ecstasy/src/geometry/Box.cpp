#include <geometry/Box.hpp>
#include <common/utils.hpp>
#include <filament/Material.h>

#include <utils/EntityManager.h>
#include <filament/Scene.h>

#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/RenderableManager.h>
#include <Eigen/Dense>

#include <material/Material.hpp>
#include <manager/RendererResourceManager.hpp>

ecstasy::Box::Box(filament::Engine& _filament_engine, RendererResourceManager& _renderer_resource_manager,
                  ecstasy::Box::Options _options)
    : filament_engine_(_filament_engine), renderer_resource_manager_{_renderer_resource_manager},
      Transformable(_filament_engine.getTransformManager()) {
    const Eigen::Vector3f half_dim = _options.dimention / 2.0f;
    auto& hd = half_dim;
    auto& d = _options.dimention;

    const auto PI_H = std::numbers::pi / 2.0;
    const auto PI = std::numbers::pi;
    const auto PID = std::numbers::pi * 2;

    log::info("{}, {}", d.z(), d.y());

    dbg_wall =
        new Plane(filament_engine_, renderer_resource_manager_,
                  Plane::GeometryOptions{.dimention = {1, 1}, .segments = {1, 1}}, _options.px_materials_);
    px_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.z(), d.y()}, .segments = {1, 1}},
                        _options.px_materials_);
    nx_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.z(), d.y()}, .segments = {1, 1}},
                        _options.nx_materials_);
    py_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.x(), d.z()}, .segments = {1, 1}},
                        _options.py_materials_);
    ny_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.x(), d.z()}, .segments = {1, 1}},
                        _options.ny_materials_);
    pz_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.x(), d.y()}, .segments = {1, 1}},
                        _options.pz_materials_);
    nz_wall = new Plane(filament_engine_, renderer_resource_manager_,
                        Plane::GeometryOptions{.dimention = {d.x(), d.y()}, .segments = {1, 1}},
                        _options.nz_materials_);

    dbg_wall = new Plane(filament_engine_, renderer_resource_manager_,
                         Plane::GeometryOptions{.dimention = {10., 10.}, .segments = {1, 1}},
                         _options.px_materials_);

    px_wall->setParent(getTransformInstace());
    nx_wall->setParent(getTransformInstace());
    py_wall->setParent(getTransformInstace());
    ny_wall->setParent(getTransformInstace());
    pz_wall->setParent(getTransformInstace());
    nz_wall->setParent(getTransformInstace());

    px_wall->setTranslationX(hd.x());
    nx_wall->setTranslationX(-hd.x());
    py_wall->setTranslationY(hd.y());
    ny_wall->setTranslationY(-hd.y());
    pz_wall->setTranslationZ(hd.z());
    nz_wall->setTranslationZ(-hd.z());

    px_wall->setRotationY(-PI_H);
    nx_wall->setRotationY(PI_H);
    py_wall->setRotationX(PI_H);
    ny_wall->setRotationX(-PI_H);
    pz_wall->setRotationY(PI);
    nz_wall->setRotationY(0);

    px_wall->computeAndSetTransform();
    nx_wall->computeAndSetTransform();
    py_wall->computeAndSetTransform();
    ny_wall->computeAndSetTransform();
    pz_wall->computeAndSetTransform();
    nz_wall->computeAndSetTransform();
    dbg_wall->computeAndSetTransform();
}

void ecstasy::Box::addRenderablesToScene(filament::Scene& _scene) {
    _scene.addEntity(px_wall->getEntity());
    _scene.addEntity(nx_wall->getEntity());
    // _scene.addEntity(py_wall->getEntity());
    // _scene.addEntity(ny_wall->getEntity());
    // _scene.addEntity(pz_wall->getEntity());
    _scene.addEntity(nz_wall->getEntity());
    _scene.addEntity(dbg_wall->getEntity());
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> ecstasy::Box::getBoundingBox() { return {}; }

ecstasy::Box::~Box() {
    // We don't own the material, only instances
    filament_engine_.destroy(renderable_);
}