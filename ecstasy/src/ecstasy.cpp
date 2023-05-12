#include <glfw3webgpu.h>
#include <common/webgpu_compat.h>

#include <GLFW/glfw3.h>

#include <webgpu/webgpu.h>

#include <cassert>
#include <iostream>
#include <vector>

#include <ecstasy/ecstasy.hpp>
#include <ecstasy/scene.hpp>
#include <Eigen/Dense>
#include "./webgpu_adapter.hpp"
#include "./webgpu_device.hpp"

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

    // We describe the Swap Chain that is used to present rendered textures on
    // screen. Note that it is specific to a given window size so don't resize.
    // Like buffers, textures are allocated for a specific usage. In our case,
    // we will use them as the target of a Render Pass so it needs to be
    // created with the `RenderAttachment` usage flag. The swap chain textures
    // use the color format suggested by the target surface. FIFO stands for
    // "first in, first out", meaning that the presented texture is always the
    // oldest one, like a regular queue.
    WGPUSwapChainDescriptor swapChainDesc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = swapChainFormat,
        .width = window_width_,
        .height = window_height_,
        .presentMode = WGPUPresentMode_Fifo};

    // Finally create the Swap Chain
    webgpu_swapchain_ =
        wgpuDeviceCreateSwapChain(webgpu_device_, surface, &swapChainDesc);

    std::cout << "Swapchain: " << webgpu_swapchain_ << std::endl;
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
    // Getting the texture may fail, in particular if the window has been
    // resized and thus the target surface changed.
    if (!nextTexture) {
        std::cerr << "Cannot acquire next swap chain texture" << std::endl;
        // break;
        return;
    }
    // std::cout << "nextTexture: " << nextTexture << std::endl;

    // The attachment is tighed to the view returned by the swap chain, so
    // that the render pass draws directly on screen.
    // resolveTarget: Not relevant here because we do not use multi-sampling
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

    // Describe a render pass, which targets the texture view
    // No depth buffer for now
    // We do not use timers for now neither
    WGPURenderPassDescriptor renderPassDesc = {
        .nextInChain = nullptr,
        .colorAttachmentCount = 1,
        .colorAttachments = &renderPassColorAttachment,
        .depthStencilAttachment = nullptr,
        .timestampWriteCount = 0,
        .timestampWrites = nullptr,
    };

    // Create a render pass. We end it immediately because we use its
    // built-in mechanism for clearing the screen when it begins (see
    // descriptor).
    WGPURenderPassEncoder renderPass =
        wgpuCommandEncoderBeginRenderPass(webgpu_encoder_, &renderPassDesc);
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