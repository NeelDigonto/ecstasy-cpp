#pragma once
#include <variant>
#include <optional>
#include <string>
#include <type_traits>

#include <Eigen/Dense>

#include <common/common.hpp>
#include <common/filament.hpp>

namespace ecstasy {
class MaterialManager;

class Material {
  public:
    struct LitOptions {
        std::variant<Eigen::Vector4f, FilePath> albedo =
            Eigen::Vector4f{102 / 255., 51 / 255., 153 / 255., 1.};
        float anisotropy = 0.7f;
        std::variant<float, FilePath> ao = 1.0f;
        std::optional<FilePath> bentNormalMap;
        float clearCoat = 1.0f;
        std::optional<FilePath> heightMap;
        std::variant<float, FilePath> metallic = 0.0f;
        std::optional<FilePath> normalMap;
        std::variant<float, FilePath> roughness = 0.4f;

        bool operator==(const LitOptions& rhs) const {
            return (albedo == rhs.albedo) && (anisotropy == rhs.anisotropy) && (ao == rhs.ao) &&
                   (bentNormalMap == rhs.bentNormalMap) && (clearCoat == rhs.clearCoat) &&
                   (heightMap == rhs.heightMap) && (metallic == rhs.metallic) &&
                   (normalMap == rhs.normalMap) && (roughness == rhs.roughness);
        }
    };

    using Options = std::variant<LitOptions>;

  private:
    filament::Engine& filament_engine_;
    MaterialManager& material_manager_;
    Options options_;
    filament::Material* material_;
    std::string name_;

  public:
    Material(filament::Engine& _filament_engine, MaterialManager& _material_manager, Options _options,
             const std::string& _name = "untitled");

    void generateLitShaderSource(const Material::LitOptions& _lit_options);
    void loadLitTextures(const Material::LitOptions& _lit_options);

    filament::MaterialInstance* createInstance(const std::string& _name = "untitled");
    filament::MaterialInstance* createLitInstance(const std::string& _name,
                                                  const Material::LitOptions& _lit_options);

    ~Material();
};
} // namespace ecstasy

template <typename T> struct std::hash<Eigen::Vector4<T>> {
    std::size_t operator()(const Eigen::Vector4<T>& _vec4) const {
        return std::hash<T>{}(_vec4.x()) ^ std::hash<T>{}(_vec4.y()) ^ std::hash<T>{}(_vec4.z()) ^
               std::hash<T>{}(_vec4.w());
    }
};

template <> struct std::hash<ecstasy::Material::Options> {
    std::size_t operator()(const ecstasy::Material::Options& _options) const {
        return std::visit(
            [](auto&& arg) -> std::size_t {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, ecstasy::Material::LitOptions>)
                    return std::hash<decltype(arg.albedo)>{}(arg.albedo) ^
                           std::hash<decltype(arg.anisotropy)>{}(arg.anisotropy) ^
                           std::hash<decltype(arg.ao)>{}(arg.ao) ^
                           std::hash<decltype(arg.bentNormalMap)>{}(arg.bentNormalMap) ^
                           std::hash<decltype(arg.clearCoat)>{}(arg.clearCoat) ^
                           std::hash<decltype(arg.heightMap)>{}(arg.heightMap) ^
                           std::hash<decltype(arg.metallic)>{}(arg.metallic) ^
                           std::hash<decltype(arg.normalMap)>{}(arg.normalMap) ^
                           std::hash<decltype(arg.roughness)>{}(arg.roughness);
                else
                    static_assert(ecstasy::always_false_v<T>, "Unsupported material option!");
            },
            _options);
    }
};