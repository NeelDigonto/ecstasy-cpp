#include <string>

namespace ecstasy {
namespace shader {

std::string simple = R"MS_START(
material {
    name : Lit,
    shadingModel : lit,
    parameters : [
        { type : float3, name : baseColor },
        { type : float,  name : roughness },
        { type : float,  name : clearCoat },
        { type : float,  name : clearCoatRoughness }
    ],
}

fragment {
    void material(inout MaterialInputs material) {
        prepareMaterial(material);
        material.baseColor.rgb = materialParams.baseColor;
        material.roughness = materialParams.roughness;
        material.clearCoat = materialParams.clearCoat;
        material.clearCoatRoughness = materialParams.clearCoatRoughness;
    }
}
)MS_START";

}
} // namespace ecstasy
