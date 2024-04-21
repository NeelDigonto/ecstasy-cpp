#include <filesystem>
#include <exception>
#include <variant>

#include <filament/Engine.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filamat/MaterialBuilder.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>
#include "stb_image.h"

#include <material/Material.hpp>
#include <manager/RendererResourceManager.hpp>

namespace ecstasy {
Material::Material(filament::Engine& _filament_engine, RendererResourceManager& _renderer_resource_manager,
                   Material::Options _options, const std::string& _name)
    : filament_engine_{_filament_engine}, renderer_resource_manager_{_renderer_resource_manager},
      options_{_options}, name_{_name} {

    std::visit(
        [this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LitOptions>) {
                generateLitShaderSource(arg);
                loadLitTextures(arg);
            } else
                static_assert(ecstasy::always_false_v<T>, "Unsupported material option!");
        },
        options_);
}

void Material::generateLitShaderSource(const Material::LitOptions& _lit_options) {
    filamat::MaterialBuilder::init();
    filamat::MaterialBuilder builder;
    builder
        .name(name_.c_str())
#ifndef NDEBUG
        .optimization(filamat::MaterialBuilderBase::Optimization::NONE)
        .generateDebugInfo(true)
#endif
        ;

    std::string shader = R"SHADER(
          void material(inout MaterialInputs material) {
        )SHADER";

    bool hasUV = false;

    std::string shader_post_text;

    if (_lit_options.heightMap.has_value()) {
        // Parallax Occlusion Mapping
        shader_post_text += R"SHADER(
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
        hasUV = true;
    }

    if (_lit_options.normalMap.has_value()) {
        shader_post_text += R"SHADER(
            material.normal = texture(materialParams_normalMap, uv0).xyz * 2.0 - 1.0;
            material.normal.y *= -1.0;
        )SHADER";

        builder.parameter("normalMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    if (_lit_options.bentNormalMap.has_value()) {
        shader_post_text += R"SHADER(
            material.bentNormal = texture(materialParams_bentNormalMap, uv0).xyz * 2.0 - 1.0;
            material.bentNormal.y *= -1.0;
        )SHADER";

        builder.parameter("bentNormalMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    shader_post_text += R"SHADER(
            prepareMaterial(material);
        )SHADER";

    if (std::holds_alternative<Eigen::Vector4f>(_lit_options.albedo)) {
        shader_post_text += R"SHADER(
            material.baseColor = materialParams.albedo;
        )SHADER";

        builder.parameter("albedo", filamat::MaterialBuilder::AttributeType::FLOAT4);
    } else if (std::holds_alternative<FilePath>(_lit_options.albedo)) {
        shader_post_text += R"SHADER(
            material.baseColor.rgb = texture(materialParams_baseColorMap, uv0).rgb;
            )SHADER";

        builder.parameter("baseColorMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    if (std::holds_alternative<float>(_lit_options.metallic)) {
        shader_post_text += R"SHADER(
            material.metallic = materialParams.metallic;
            )SHADER";

        builder.parameter("metallic", filamat::MaterialBuilder::AttributeType::FLOAT);
    } else if (std::holds_alternative<FilePath>(_lit_options.metallic)) {
        shader_post_text += R"SHADER(
            material.metallic = texture(materialParams_metallicMap, uv0).r;
            )SHADER";

        builder.parameter("metallicMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    if (std::holds_alternative<float>(_lit_options.roughness)) {
        shader_post_text += R"SHADER(
            material.roughness = materialParams.roughness;
            )SHADER";

        builder.parameter("roughness", filamat::MaterialBuilder::AttributeType::FLOAT);
    } else if (std::holds_alternative<FilePath>(_lit_options.roughness)) {
        shader_post_text += R"SHADER(
            material.roughness = texture(materialParams_roughnessMap, uv0).r;
            )SHADER";

        builder.parameter("roughnessMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    if (std::holds_alternative<float>(_lit_options.ao)) {
        shader_post_text += R"SHADER(
            material.ambientOcclusion = materialParams.ambientOcclusion;
            )SHADER";

        builder.parameter("ambientOcclusion", filamat::MaterialBuilder::AttributeType::FLOAT);
    } else if (std::holds_alternative<FilePath>(_lit_options.ao)) {
        shader_post_text += R"SHADER(
            material.ambientOcclusion = texture(materialParams_aoMap, uv0).r;
            )SHADER";

        builder.parameter("aoMap", filamat::MaterialBuilder::SamplerType::SAMPLER_2D);
        hasUV = true;
    }

    {
        shader_post_text += R"SHADER(
            material.clearCoat = materialParams.clearCoat;
            )SHADER";

        builder.parameter("clearCoat", filamat::MaterialBuilder::AttributeType::FLOAT);
    }

    {
        shader_post_text += R"SHADER(
            material.anisotropy = materialParams.anisotropy;
            )SHADER";

        builder.parameter("anisotropy", filamat::MaterialBuilder::AttributeType::FLOAT);
    }

    shader_post_text += "}\n";

    if (hasUV) {
        shader += R"SHADER(
            vec2 uv0 = getUV0();
        )SHADER";

        builder.require(filamat::MaterialBuilder::VertexAttribute::UV0);
    }

    shader += shader_post_text;

    log::info("Generated Shader: \n{}", shader);

    builder.material(shader.c_str())
        .doubleSided(false)
        .multiBounceAmbientOcclusion(true)
        .specularAmbientOcclusion(filamat::MaterialBuilder::SpecularAmbientOcclusion::BENT_NORMALS)
        .shading(filamat::MaterialBuilder::Shading::LIT)
        .require(filamat::MaterialBuilder::VertexAttribute::POSITION)
        .require(filamat::MaterialBuilder::VertexAttribute::TANGENTS)
        .targetApi(filamat::MaterialBuilder::TargetApi::OPENGL)
        .platform(filamat::MaterialBuilder::Platform::DESKTOP)
        .optimization(filamat::MaterialBuilderBase::Optimization::PREPROCESSOR);

    material_ = renderer_resource_manager_.getMaterial(builder, options_);
}

void Material::loadLitTextures(const Material::LitOptions& _lit_options) {
    if (std::holds_alternative<FilePath>(_lit_options.albedo)) {
        renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.albedo), true);
    }

    if (std::holds_alternative<FilePath>(_lit_options.ao)) {
        renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.ao), false);
    }

    if (_lit_options.bentNormalMap.has_value()) {
        renderer_resource_manager_.getTexture(_lit_options.bentNormalMap.value(), false);
    }

    if (_lit_options.heightMap.has_value()) {
        renderer_resource_manager_.getTexture(_lit_options.heightMap.value(), false);
    }

    if (std::holds_alternative<FilePath>(_lit_options.metallic)) {
        renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.metallic), false);
    }

    if (_lit_options.normalMap.has_value()) {
        renderer_resource_manager_.getTexture(_lit_options.normalMap.value(), false);
    }

    if (std::holds_alternative<FilePath>(_lit_options.roughness)) {
        renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.roughness), false);
    }
}

filament::MaterialInstance* Material::createInstance(const std::string& _name) {
    return std::visit(
        [this, &_name](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LitOptions>) {
                return createLitInstance(_name, arg);
            } else
                static_assert(ecstasy::always_false_v<T>, "Unsupported material option!");
        },
        options_);
}

filament::MaterialInstance* Material::createLitInstance(const std::string& _name,
                                                        const Material::LitOptions& _lit_options) {
    auto instance = material_->createInstance(_name.c_str());

    filament::TextureSampler sampler(filament::TextureSampler::MinFilter::LINEAR_MIPMAP_LINEAR,
                                     filament::TextureSampler::MagFilter::LINEAR,
                                     filament::TextureSampler::WrapMode::REPEAT);
    sampler.setAnisotropy(8.0f);

    if (_lit_options.heightMap.has_value()) {
        const auto texture = renderer_resource_manager_.getTexture(_lit_options.heightMap.value(), false);
        instance->setParameter("heightMap", texture, sampler);
    }

    if (_lit_options.normalMap.has_value()) {
        const auto texture = renderer_resource_manager_.getTexture(_lit_options.normalMap.value(), false);
        instance->setParameter("normalMap", texture, sampler);
    }

    if (_lit_options.bentNormalMap.has_value()) {
        const auto texture = renderer_resource_manager_.getTexture(_lit_options.bentNormalMap.value(), false);
        instance->setParameter("bentNormalMap", texture, sampler);
    }

    if (std::holds_alternative<Eigen::Vector4f>(_lit_options.albedo)) {
        const auto albedo = std::get<Eigen::Vector4f>(_lit_options.albedo);
        instance->setParameter("albedo",
                               filament::math::float4{albedo.x(), albedo.y(), albedo.z(), albedo.w()});
    } else if (std::holds_alternative<FilePath>(_lit_options.albedo)) {
        const auto texture =
            renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.albedo), true);
        instance->setParameter("baseColorMap", texture, sampler);
    }

    if (std::holds_alternative<float>(_lit_options.metallic)) {
        const auto metallic = std::get<float>(_lit_options.metallic);
        instance->setParameter("metallic", metallic);
    } else if (std::holds_alternative<FilePath>(_lit_options.metallic)) {
        const auto texture =
            renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.metallic), true);
        instance->setParameter("metallicMap", texture, sampler);
    }

    if (std::holds_alternative<float>(_lit_options.roughness)) {
        const auto roughness = std::get<float>(_lit_options.roughness);
        instance->setParameter("roughness", roughness);
    } else if (std::holds_alternative<FilePath>(_lit_options.roughness)) {
        const auto texture =
            renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.roughness), true);
        instance->setParameter("roughnessMap", texture, sampler);
    }

    if (std::holds_alternative<float>(_lit_options.ao)) {
        const auto ambientOcclusion = std::get<float>(_lit_options.ao);
        instance->setParameter("ambientOcclusion", ambientOcclusion);
    } else if (std::holds_alternative<FilePath>(_lit_options.ao)) {
        const auto texture = renderer_resource_manager_.getTexture(std::get<FilePath>(_lit_options.ao), true);
        instance->setParameter("aoMap", texture, sampler);
    }

    return instance;
}

Material::~Material() {
    filament_engine_.destroy(material_);
    material_->createInstance();
}
} // namespace ecstasy