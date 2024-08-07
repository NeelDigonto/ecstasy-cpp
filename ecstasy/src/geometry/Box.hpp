#pragma once
#include <common/common.hpp>
#include <utils/Entity.h>
#include <filament/TransformManager.h>
#include <geometry/Transformable.hpp>

namespace filament {
class Engine;
class VertexBuffer;
class IndexBuffer;
class MaterialInstance;
class Scene;
} // namespace filament

namespace ecstasy {
class Material;
class RendererResourceManager;
class Plane;

class Box : public Transformable {
  public:
    struct Options {
        Eigen::Vector3f dimention = Eigen::Vector3f{1., 1., 1.};
        ecstasy::Material* px_materials_;
        ecstasy::Material* nx_materials_;
        ecstasy::Material* py_materials_;
        ecstasy::Material* ny_materials_;
        ecstasy::Material* pz_materials_;
        ecstasy::Material* nz_materials_;

        bool operator==(const Options& rhs) const { return (dimention == rhs.dimention); }
    };

  private:
    filament::Engine& filament_engine_;
    RendererResourceManager& renderer_resource_manager_;
    Plane* px_wall;
    Plane* nx_wall;
    Plane* py_wall;
    Plane* ny_wall;
    Plane* pz_wall;
    Plane* nz_wall;
    utils::Entity renderable_;

  public:
    Box() = delete;
    Box(filament::Engine& _filament_engine, RendererResourceManager& renderer_resource_manager_,
        Options _geometry_options);
    std::pair<Eigen::Vector3d, Eigen::Vector3d> getBoundingBox();
    utils::Entity getRenderable() { return renderable_; }
    void addRenderablesToScene(filament::Scene& _scene);
    ~Box();
};
} // namespace ecstasy
