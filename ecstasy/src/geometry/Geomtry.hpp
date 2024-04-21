#pragma once
#include <ecstasy/ecstasy.hpp>
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

struct PlaneOptions {
    Eigen::Vector4f dimention;
    Material* material;

    bool operator==(const PlaneOptions& rhs) const { return (dimention == rhs.dimention); }
};

using Options = std::variant<PlaneOptions>;

class plane {
  private:
    static constexpr size_t WIREFRAME_OFFSET = 3 * 2 * 6;

    std::vector<Eigen::Vector3f> vertices_;
    std::vector<Eigen::Vector4f> normals_;
    std::vector<Eigen::Vector2f> uvs_;
    std::vector<std::uint32_t> indices_;

    filament::Engine& filament_engine_;
    filament::VertexBuffer* vertex_buffer_ = nullptr;
    filament::IndexBuffer* index_buffer_ = nullptr;
    Material* material_ = nullptr;
    filament::MaterialInstance* material_instance_ = nullptr;
    utils::Entity renderable_;

  public:
    plane() = delete;
    plane(filament::Engine& _filament_engine, Eigen::Vector3d _dimention, ecstasy::Material* _material,
          Eigen::Vector3d _linear_color, const Eigen::Vector3f& _translation,
          const Eigen::Vector3f& _rotation, bool _culling = true);
    std::pair<Eigen::Vector3d, Eigen::Vector3d> getBoundingBox();

    utils::Entity getRenderable() { return renderable_; }

    ~plane();
};
} // namespace ecstasy
