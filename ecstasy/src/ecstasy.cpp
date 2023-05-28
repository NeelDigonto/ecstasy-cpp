#include <glfw3webgpu.h>
#include <common/webgpu_compat.h>

#include <GLFW/glfw3.h>

// #define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.h>

#include <cassert>
#include <iostream>
#include <vector>

#include <ecstasy/ecstasy.hpp>
#include <ecstasy/scene.hpp>
#include <Eigen/Dense>
#include "./webgpu_adapter.hpp"
#include "./webgpu_device.hpp"
#include <chrono>
// https : // github.com/BinomialLLC/basis_universal
ecstasy::app::app(std::string _app_name, std::uint32_t _window_width,
                  std::uint32_t _window_height) {
    app_name_ = _app_name;
    window_height_ = _window_height;
    window_width_ = _window_width;

    WGPUInstanceDescriptor desc = {.nextInChain = nullptr};
    webgpu_instance_ = wgpuCreateInstance(&desc);
    if (!webgpu_instance_) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return;
    }

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW!" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(window_width_, window_height_, app_name_.c_str(),
                               NULL, NULL);
    if (!window_) {
        std::cerr << "Could not open window!" << std::endl;
        return;
    }

    WGPUSurface surface = glfwGetWGPUSurface(webgpu_instance_, window_);

    std::cout << "Requesting adapter..." << std::endl;
    WGPURequestAdapterOptions adapterOpts = {.nextInChain = nullptr,
                                             .compatibleSurface = surface};
    webgpu_adapter_ = requestAdapter(webgpu_instance_, &adapterOpts);
    std::cout << "Got adapter: " << webgpu_adapter_ << std::endl;

    std::cout << "Requesting device..." << std::endl;
    WGPUDeviceDescriptor deviceDesc = {
        .nextInChain = nullptr,
        .label = "My Device",
        .requiredFeaturesCount = 0,
        .requiredLimits = nullptr,
        .defaultQueue = {.nextInChain = nullptr, .label = "The default queue"}};
    webgpu_device_ = requestDevice(webgpu_adapter_, &deviceDesc);
    std::cout << "Got device: " << webgpu_device_ << std::endl;

    webgpu_queue_ = wgpuDeviceGetQueue(webgpu_device_);

    std::cout << "Creating swapchain device..." << std::endl;

    WGPUTextureFormat swapChainFormat =
        wgpuSurfaceGetPreferredFormat(surface, webgpu_adapter_);

    WGPUSwapChainDescriptor swapChainDesc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = swapChainFormat,
        .width = window_width_,
        .height = window_height_,
        .presentMode = WGPUPresentMode_Mailbox};

    // Finally create the Swap Chain
    webgpu_swapchain_ =
        wgpuDeviceCreateSwapChain(webgpu_device_, surface, &swapChainDesc);

    std::cout << "Swapchain: " << webgpu_swapchain_ << std::endl;

    std::cout << "Creating shader module..." << std::endl;
    const char* shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
	var p = vec2<f32>(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2<f32>(-0.5, -0.5);
	} else if (in_vertex_index == 1u) {
		p = vec2<f32>(0.5, -0.5);
	} else {
		p = vec2<f32>(0.0, 0.5);
	}
	return vec4<f32>(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4<f32> {
    return vec4<f32>(0.0, 0.4, 1.0, 1.0);
}
)";

    WGPUShaderModuleWGSLDescriptor shaderCodeDesc{
        .chain = {.next = nullptr,
                  .sType = WGPUSType_ShaderModuleWGSLDescriptor},
        .code = shaderSource};

    // Connect the chain
    WGPUShaderModuleDescriptor shaderDesc{
        .nextInChain = &shaderCodeDesc.chain, .hintCount = 0, .hints = nullptr};

    WGPUShaderModule shaderModule =
        wgpuDeviceCreateShaderModule(webgpu_device_, &shaderDesc);
    std::cout << "Shader module: " << shaderModule << std::endl;

    std::cout << "Creating render pipeline..." << std::endl;

    WGPUBlendState blendState{
        .color =
            {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_SrcAlpha,
                .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
            },
        .alpha = {
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_Zero,
            .dstFactor = WGPUBlendFactor_One,
        }};

    WGPUColorTargetState colorTarget{.nextInChain = nullptr,
                                     .format = swapChainFormat,
                                     .blend = &blendState,
                                     .writeMask = WGPUColorWriteMask_All};

    WGPUFragmentState fragmentState{.nextInChain = nullptr,
                                    .module = shaderModule,
                                    .entryPoint = "fs_main",
                                    .constantCount = 0,
                                    .constants = nullptr,
                                    .targetCount = 1,
                                    .targets = &colorTarget};

    WGPUPipelineLayoutDescriptor layoutDesc{
        .nextInChain = nullptr,
        .bindGroupLayoutCount = 0,
        .bindGroupLayouts = nullptr,
    };
    WGPUPipelineLayout layout =
        wgpuDeviceCreatePipelineLayout(webgpu_device_, &layoutDesc);

    WGPURenderPipelineDescriptor pipelineDesc{
        .nextInChain = nullptr,
        .layout = layout,
        .vertex =
            {
                .nextInChain = nullptr,
                .module = shaderModule,
                .entryPoint = "vs_main",
                .constantCount = 0,
                .constants = nullptr,
                .bufferCount = 0,
                .buffers = nullptr,
            },
        .primitive =
            {
                .nextInChain = nullptr,
                .topology = WGPUPrimitiveTopology_TriangleList,
                .stripIndexFormat = WGPUIndexFormat_Undefined,
                .frontFace = WGPUFrontFace_CCW,
                .cullMode = WGPUCullMode_None,
            },
        .depthStencil = nullptr,
        .multisample =
            {
                .count = 1,
                .mask = ~0u,
                .alphaToCoverageEnabled = false,
            },
        .fragment = &fragmentState,
    };

    render_pipeline_ =
        wgpuDeviceCreateRenderPipeline(webgpu_device_, &pipelineDesc);
    std::cout << "Render pipeline: " << render_pipeline_ << std::endl;
}

void ecstasy::app::setClearColor(const Eigen::Vector4d& _clear_color) noexcept {
    clear_color_ = _clear_color;
};

bool ecstasy::app::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
}

/* ecstasy::scene ecstasy::app::createScene() {
    // registry_.emplace<>;
    std::cout << "Scene Created! " << std::endl;
} */

void ecstasy::app::animate() {
    glfwPollEvents();

    // Get the texture where to draw the next frame
    WGPUTextureView nextTexture =
        wgpuSwapChainGetCurrentTextureView(webgpu_swapchain_);

    if (!nextTexture) {
        std::cerr << "Cannot acquire next swap chain texture" << std::endl;
        // break;
        return;
    }
    const WGPUColor clear_color = {clear_color_[0], clear_color_[1],
                                   clear_color_[2], clear_color_[3]};

    WGPURenderPassColorAttachment renderPassColorAttachment = {
        .view = nextTexture,
        .resolveTarget = nullptr,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = clear_color};

    WGPUCommandEncoderDescriptor commandEncoderDesc = {
        .nextInChain = nullptr,
        .label = "Command Encoder",
    };
    // needs to be in render loop
    webgpu_encoder_ =
        wgpuDeviceCreateCommandEncoder(webgpu_device_, &commandEncoderDesc);

    WGPURenderPassDescriptor renderPassDesc = {
        .nextInChain = nullptr,
        .colorAttachmentCount = 1,
        .colorAttachments = &renderPassColorAttachment,
        .depthStencilAttachment = nullptr,
        .timestampWriteCount = 0,
        .timestampWrites = nullptr,
    };

    WGPURenderPassEncoder renderPass =
        wgpuCommandEncoderBeginRenderPass(webgpu_encoder_, &renderPassDesc);

    // ----
    // In its overall outline, drawing a triangle is as simple as this:
    // Select which render pipeline to use
    wgpuRenderPassEncoderSetPipeline(renderPass, render_pipeline_);
    // Draw 1 instance of a 3-vertices shape
    wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
    // ---
    wgpuRenderPassEncoderEnd(renderPass);

    wgpuTextureViewRelease(nextTexture);

    WGPUCommandBufferDescriptor cmdBufferDescriptor = {
        .nextInChain = nullptr, .label = "Command buffer"};
    WGPUCommandBuffer command =
        wgpuCommandEncoderFinish(webgpu_encoder_, &cmdBufferDescriptor);
    wgpuQueueSubmit(webgpu_queue_, 1, &command);

    // We can tell the swap chain to present the next texture.
    wgpuSwapChainPresent(webgpu_swapchain_);
}

ecstasy::app::~app() {
    wgpuSwapChainRelease(webgpu_swapchain_);
    wgpuDeviceRelease(webgpu_device_);
    wgpuAdapterRelease(webgpu_adapter_);
    wgpuInstanceRelease(webgpu_instance_);
    glfwDestroyWindow(window_);
    glfwTerminate();
}

//  The exceptions are GPUCommandBuffer, GPURenderPassEncoder and
//  GPUComputePassEncoder which you're required to make every frame but don't
//  worry about it, they are GCed