#include <string>
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filamat/MaterialBuilder.h>

namespace ecstasy {
namespace shader {

void simple(filamat::MaterialBuilder& _builder) {
    _builder.name("Simple Lit Shader")
        .material(R"MS_START(
            void material(inout MaterialInputs material) {
                prepareMaterial(material);
                material.baseColor.rgb = materialParams.baseColor;
                material.metallic = materialParams.metallic;
                material.roughness = materialParams.roughness;
                material.reflectance = materialParams.reflectance;
            }
        )MS_START")
        .parameter("baseColor", filament::backend::UniformType::FLOAT3)
        .parameter("metallic", filament::backend::UniformType::FLOAT)
        .parameter("roughness", filament::backend::UniformType::FLOAT)
        .parameter("reflectance", filament::backend::UniformType::FLOAT)
        .shading(filamat::MaterialBuilder::Shading::LIT)
        .require(filamat::MaterialBuilder::VertexAttribute::POSITION)
        .require(filamat::MaterialBuilder::VertexAttribute::TANGENTS)
        .require(filamat::MaterialBuilder::VertexAttribute::UV0)
        .doubleSided(false)
        .targetApi(filamat::MaterialBuilder::TargetApi::OPENGL)
        .platform(filamat::MaterialBuilder::Platform::DESKTOP);
}

struct PBROptions {
    std::optional<double> aoMap = 1.0;
    std::optional<Eigen::Vector3d> baseColorMap = Eigen::Vector3d{1.0, 1.0, 1.0};
    std::optional<bool> bentNormalMap = false;
    std::optional<bool> heightMap = false;
    std::optional<double> metallicMap = 0.0;
    std::optional<bool> normalMap = false;
    std::optional<double> roughnessMap = 0.4;
    std::optional<double> clearCoat = 1.0;
    std::optional<double> anisotropy = 0.7;
};

filament::Material* pbr(filament::Engine& _filament_engine, std::string _name, const PBROptions _options) {
    filamat::MaterialBuilder::init();
    filamat::MaterialBuilder builder;
    builder
        .name(_name.c_str())
#ifndef NDEBUG
        .optimization(filamat::MaterialBuilderBase::Optimization::NONE)
        .generateDebugInfo(true)
#endif
        ;

    std::string shader = R"SHADER(
        void material(inout MaterialInputs material) {
    )SHADER";

    const bool hasUV = _options.aoMap || _options.baseColorMap || _options.bentNormalMap ||
                       _options.heightMap || _options.metallicMap || _options.normalMap ||
                       _options.roughnessMap;

    if (hasUV) {
        shader += R"SHADER(
            vec2 uv0 = getUV0();
        )SHADER";

        builder.require(filamat::MaterialBuilder::VertexAttribute::UV0);
    }

    if (_options.heightMap.has_value() && _options.heightMap.value()) {
        // Parallax Occlusion Mapping
        shader += R"SHADER(
            vec2 uvDx = dFdx(uv0);
            vec2 uvDy = dFdy(uv0);

            mat3 tangentFromWorld = transpose(getWorldTangentFrame());
            vec3 v = tangentFromWorld * getWorldViewVector();

            float minLayers = 8.0;
            float maxLayers = 48.0;
            float numLayers = mix(maxLayers, minLayers,
                    dot(getWorldGeometricNormalVector(), getWorldViewVector()));
            float heightScale = 0.05;

            float layerDepth = 1.0 / numLayers;
            float currLayerDepth = 0.0;

            vec2 deltaUV = v.xy * heightScale / (v.z * numLayers);
            vec2 currUV = uv0;
            float height = 1.0 - textureGrad(materialParams_heightMap, currUV, uvDx, uvDy).r;
            for (int i = 0; i < int(numLayers); i++) {
                currLayerDepth += layerDepth;
                currUV -= deltaUV;
                height = 1.0 - textureGrad(materialParams_heightMap, currUV, uvDx, uvDy).r;
                if (height < currLayerDepth) {
                    break;
                }
            }

            vec2 prevUV = currUV + deltaUV;
            float nextDepth = height - currLayerDepth;
            float prevDepth = 1.0 - textureGrad(materialParams_heightMap, prevUV, uvDx, uvDy).r -
                    currLayerDepth + layerDepth;
            uv0 = mix(currUV, prevUV, nextDepth / (nextDepth - prevDepth));
        )SHADER";

        builder.parameter("heightMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    }

    if (_options.normalMap.has_value() && _options.normalMap.value()) {
        shader += R"SHADER(
            material.normal = texture(materialParams_normalMap, uv0).xyz * 2.0 - 1.0;
            material.normal.y *= -1.0;
        )SHADER";

        builder.parameter("normalMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    }

    if (_options.bentNormalMap.has_value() && _options.bentNormalMap.value()) {
        shader += R"SHADER(
            material.bentNormal = texture(materialParams_bentNormalMap, uv0).xyz * 2.0 - 1.0;
            material.bentNormal.y *= -1.0;
        )SHADER";

        builder.parameter("bentNormalMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    }

    shader += R"SHADER(
            prepareMaterial(material);
    )SHADER";

    if (!_options.baseColorMap.has_value()) {
        shader += R"SHADER(
            material.baseColor.rgb = texture(materialParams_baseColorMap, uv0).rgb;
        )SHADER";

        builder.parameter("baseColorMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    } else {
        shader += fmt::format(R"SHADER(
            material.baseColor.rgb = float3({:f}, {:f}, {:f});
        )SHADER",
                              _options.baseColorMap.value().x(), _options.baseColorMap.value().y(),
                              _options.baseColorMap.value().z());
    }

    if (!_options.metallicMap.has_value()) {
        shader += R"SHADER(
            material.metallic = texture(materialParams_metallicMap, uv0).r;
        )SHADER";

        builder.parameter("metallicMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    } else {
        shader += fmt::format(R"SHADER(
            material.metallic = {:f};
        )SHADER",
                              _options.metallicMap.value());
    }

    if (!_options.roughnessMap.has_value()) {
        shader += R"SHADER(
            material.roughness = texture(materialParams_roughnessMap, uv0).r;
        )SHADER";

        builder.parameter("roughnessMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    } else {
        shader += fmt::format(R"SHADER(
            material.roughness = {:f};
        )SHADER",
                              _options.roughnessMap.value());
    }

    if (!_options.aoMap.has_value()) {
        shader += shader += R"SHADER(
            material.ambientOcclusion = texture(materialParams_aoMap, uv0).r;
        )SHADER";

        builder.parameter("aoMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
    } else {
        shader += fmt::format(R"SHADER(
            material.ambientOcclusion = {:f};
        )SHADER",
                              _options.aoMap.value());
    }

    if (_options.clearCoat.has_value()) {
        shader += fmt::format(R"SHADER(
            material.clearCoat = {:f};
        )SHADER",
                              _options.aoMap.value());
    } else {
        shader += shader += R"SHADER(
            material.clearCoat = 1.0;
        )SHADER";
    }

    if (_options.anisotropy.has_value()) {
        shader += fmt::format(R"SHADER(
            material.anisotropy = {:f};
        )SHADER",
                              _options.anisotropy.value());
    } else {
        shader += R"SHADER(
            material.anisotropy = 0.7;
        )SHADER";
    }
    shader += "}\n";

    log::info("{}", shader);

    builder.material(shader.c_str())
        .doubleSided(false)
        .multiBounceAmbientOcclusion(true)
        .specularAmbientOcclusion(filamat::MaterialBuilder::SpecularAmbientOcclusion::BENT_NORMALS)
        .shading(filamat::MaterialBuilder::Shading::LIT)
        .require(filamat::MaterialBuilder::VertexAttribute::POSITION)
        .require(filamat::MaterialBuilder::VertexAttribute::TANGENTS)
        .targetApi(filamat::MaterialBuilder::TargetApi::OPENGL)
        .platform(filamat::MaterialBuilder::Platform::DESKTOP)
        .optimization(filamat::MaterialBuilderBase::Optimization::PERFORMANCE);

    filamat::Package package = builder.build(_filament_engine.getJobSystem());

    filament::Material* material =
        filament::Material::Builder().package(package.getData(), package.getSize()).build(_filament_engine);

    return material;
}

} // namespace shader
} // namespace ecstasy
