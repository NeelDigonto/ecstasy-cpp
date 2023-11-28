#include <string>
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
        .doubleSided(true)
        .targetApi(filamat::MaterialBuilder::TargetApi::OPENGL)
        .platform(filamat::MaterialBuilder::Platform::DESKTOP);
}

} // namespace shader
} // namespace ecstasy
