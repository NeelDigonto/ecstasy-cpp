#pragma once
#include <common/common.hpp>
#include <utils/Entity.h>

namespace filament {
class Engine;
class VertexBuffer;
class IndexBuffer;
class Material;
class MaterialInstance;
} // namespace filament

namespace ecstasy {
class Material;
class RendererResourceManager;

class Plane {
  public:
    struct GeometryOptions {
        Eigen::Vector3f dimention = Eigen::Vector3f{1., 1., 1.};
        Eigen::Vector3f segments = Eigen::Vector3f{1., 1., 1.};

        bool operator==(const GeometryOptions& rhs) const {
            return (dimention == rhs.dimention) && (segments == rhs.segments);
        }
    };

    struct GeometryData {
        filament::VertexBuffer* vertex_buffer;
        filament::IndexBuffer* index_buffer;
    };

  private:
    filament::Engine& filament_engine_;
    RendererResourceManager& renderer_resource_manager_;
    GeometryData geometry_data_;
    Material* material_ = nullptr;
    filament::MaterialInstance* material_instance_ = nullptr;
    utils::Entity renderable_;

  public:
    Plane() = delete;
    Plane(filament::Engine& _filament_engine, RendererResourceManager& renderer_resource_manager_,
          GeometryOptions _geometry_options, ecstasy::Material* _material);

    std::pair<Eigen::Vector3d, Eigen::Vector3d> getBoundingBox();
    Plane* setTranslation(Eigen::Vector3f _translation);
    Plane* setTotation(Eigen::Vector3f _rotation);
    Plane* setScale(Eigen::Vector3f _scale);

    utils::Entity getRenderable() { return renderable_; }

    ~Plane();
};
} // namespace ecstasy

template <> struct std::hash<ecstasy::Plane::GeometryOptions> {
    std::size_t operator()(const ecstasy::Plane::GeometryOptions& _options) const {
        return std::hash<decltype(_options.dimention)>{}(_options.dimention) ^
               std::hash<decltype(_options.segments)>{}(_options.segments);
    }
};
