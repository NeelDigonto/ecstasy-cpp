#pragma once
#include <unordered_map>

#include <Eigen/Dense>

#include <common/common.hpp>
#include <common/filament.hpp>
#include <material/Material.hpp>

namespace ecstasy {
class MaterialManager {
  private:
    filament::Engine& filament_engine_;
    std::unordered_map<Material::Options, filament::Material*> material_cache_;
    std::unordered_map<std::string, filament::Texture*> texture_cache_;

  public:
    MaterialManager(filament::Engine& _filament_engine);

    filament::Material* getMaterial(filamat::MaterialBuilder& _builder, const Material::Options& _options);
    filament::Texture* getTexture(const std::string& _file_path, const bool sRGB);
};
} // namespace ecstasy