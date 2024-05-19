#pragma once
#include <unordered_map>
#include <unordered_set>

#include <Eigen/Dense>

#include <common/common.hpp>
#include <common/filament.hpp>
#include <material/Material.hpp>
#include <geometry/Plane.hpp>

namespace ecstasy {
class RendererResourceManager {
  public:
  private:
    filament::Engine& filament_engine_;
    std::unordered_map<Material::Options, filament::Material*> material_cache_;
    std::unordered_map<std::string, filament::Texture*> texture_cache_;
    std::unordered_map<Plane::GeometryOptions, Plane::GeometryData> plane_geometry_data_cache_;
    std::unordered_map<void*, std::variant<std::vector<Eigen::Vector2f>, std::vector<Eigen::Vector3f>,
                                           std::vector<Eigen::Vector4f>, std::vector<std::uint32_t>>>
        pending_delete_buffers_;

  public:
    RendererResourceManager(filament::Engine& _filament_engine);

    filament::Material* getMaterial(filamat::MaterialBuilder& _builder, const Material::Options& _options);
    filament::Texture* getTexture(const std::string& _file_path, const bool sRGB);
    Plane::GeometryData getPlaneGeometryData(const Plane::GeometryOptions& _options);
    void deleteOldBuffer(void* _buffer);
    static void removeBufferFunction(void* _buffer, size_t _size, void* _renderer_resource_manager) {
        log::debug("removing {}", _buffer);
        auto renderer_resource_manager = (RendererResourceManager*)_renderer_resource_manager;
        renderer_resource_manager->deleteOldBuffer(_buffer);
    };
};
} // namespace ecstasy