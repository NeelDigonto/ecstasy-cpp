#include <webgpu/webgpu.h>
#include <common/webgpu_compat.h>
#include <ecstasy/ecstasy.hpp>
#include <ecstasy/shader.hpp>

ecstasy::shader::shader(const std::string& _shader_source) {

    // Use the extension mechanism to load a WGSL shader source code
    // Set the chained struct's header
    // Connect the chain
    WGPUShaderModuleWGSLDescriptor shaderCodeDesc{
        .chain = {.next = nullptr,
                  .sType = WGPUSType_ShaderModuleWGSLDescriptor},
        .code = _shader_source.c_str()};

    WGPUShaderModuleDescriptor shaderDesc{
        .nextInChain = &shaderCodeDesc.chain, .hintCount = 0, .hints = nullptr};

    shader_module_ =
        wgpuDeviceCreateShaderModule(app_->webgpu_device_, &shaderDesc);
    // std::cout << "Shader module: " << shaderModule << std::endl;
}

ecstasy::shader::~shader() { wgpuShaderModuleRelease(shader_module_); }