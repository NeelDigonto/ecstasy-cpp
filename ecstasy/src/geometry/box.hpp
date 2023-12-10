#pragma once
#include <ecstasy/ecstasy.hpp>

namespace filament {
class Engine;
class VertexBuffer;
class IndexBuffer;
class Material;
class MaterialInstance;
} // namespace filament

namespace ecstasy {
class Cube {
  public:
    Cube() = delete;
    Cube(filament::Engine* _engine, Eigen::Vector3d _dimention, Eigen::Vector3d _segments);
};
} // namespace ecstasy

/*  Cube(filament::Engine& engine, filament::Material const* material, filament::math::float3 linearColor,
          bool culling = true);

     utils::Entity getSolidRenderable() { return mSolidRenderable; }
     utils::Entity getWireFrameRenderable() { return mWireFrameRenderable; }

     ~Cube();

     void mapFrustum(filament::Engine& engine, filament::Camera const* camera);
     void mapFrustum(filament::Engine& engine, filament::math::mat4 const& transform);
     void mapAabb(filament::Engine& engine, filament::Box const& box);

   private:
     static constexpr size_t WIREFRAME_OFFSET = 3 * 2 * 6;
     static const uint32_t mIndices[];
     static const filament::math::float3 mVertices[];

     filament::Engine& mEngine;
     filament::VertexBuffer* mVertexBuffer = nullptr;
     filament::IndexBuffer* mIndexBuffer = nullptr;
     filament::Material const* mMaterial = nullptr;
     filament::MaterialInstance* mMaterialInstanceSolid = nullptr;
     filament::MaterialInstance* mMaterialInstanceWireFrame = nullptr;
     utils::Entity mSolidRenderable;
     utils::Entity mWireFrameRenderable; */