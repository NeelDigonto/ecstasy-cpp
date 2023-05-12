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
    WGPUInstance instance = wgpuCreateInstance(&desc);
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return;
    }

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW!" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(window_width_, window_height_,
                                          app_name_.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Could not open window!" << std::endl;
        return;
    }

    WGPUSurface surface = glfwGetWGPUSurface(instance, window);

    std::cout << "Requesting adapter..." << std::endl;
    WGPURequestAdapterOptions adapterOpts = {.nextInChain = nullptr,
                                             .compatibleSurface = surface};
    WGPUAdapter adapter = requestAdapter(instance, &adapterOpts);
    std::cout << "Got adapter: " << adapter << std::endl;

    std::cout << "Requesting device..." << std::endl;
    WGPUDeviceDescriptor deviceDesc = {
        .nextInChain = nullptr,
        .label = "My Device",
        .requiredFeaturesCount = 0,
        .requiredLimits = nullptr,
        .defaultQueue = {.nextInChain = nullptr, .label = "The default queue"}};
    WGPUDevice device = requestDevice(adapter, &deviceDesc);
    std::cout << "Got device: " << device << std::endl;

    WGPUQueue queue = wgpuDeviceGetQueue(device);

    std::cout << "Creating swapchain device..." << std::endl;

    WGPUTextureFormat swapChainFormat =
        wgpuSurfaceGetPreferredFormat(surface, adapter);

    // We describe the Swap Chain that is used to present rendered textures on
    // screen. Note that it is specific to a given window size so don't resize.
    // Like buffers, textures are allocated for a specific usage. In our case,
    // we will use them as the target of a Render Pass so it needs to be created
    // with the `RenderAttachment` usage flag.
    // The swap chain textures use the color format suggested by the target
    // surface.
    // FIFO stands for "first in, first out", meaning that the presented
    // texture is always the oldest one, like a regular queue.
    WGPUSwapChainDescriptor swapChainDesc = {
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = swapChainFormat,
        .width = window_width_,
        .height = window_height_,
        .presentMode = WGPUPresentMode_Fifo};

    // Finally create the Swap Chain
    WGPUSwapChain swapChain =
        wgpuDeviceCreateSwapChain(device, surface, &swapChainDesc);

    std::cout << "Swapchain: " << swapChain << std::endl;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Get the texture where to draw the next frame
        WGPUTextureView nextTexture =
            wgpuSwapChainGetCurrentTextureView(swapChain);
        // Getting the texture may fail, in particular if the window has been
        // resized and thus the target surface changed.
        if (!nextTexture) {
            std::cerr << "Cannot acquire next swap chain texture" << std::endl;
            break;
        }
        std::cout << "nextTexture: " << nextTexture << std::endl;

        WGPUCommandEncoderDescriptor commandEncoderDesc = {
            .nextInChain = nullptr,
            .label = "Command Encoder",
        };

        WGPUCommandEncoder encoder =
            wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);

        // Describe a render pass, which targets the texture view
        WGPURenderPassDescriptor renderPassDesc = {};

        WGPURenderPassColorAttachment renderPassColorAttachment = {};
        // The attachment is tighed to the view returned by the swap chain, so
        // that the render pass draws directly on screen.
        renderPassColorAttachment.view = nextTexture;
        // resolveTarget: Not relevant here because we do not use multi-sampling
        renderPassColorAttachment.resolveTarget = nullptr;
        renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
        renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
        renderPassColorAttachment.clearValue = WGPUColor{0.9, 0.1, 0.2, 1.0};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &renderPassColorAttachment;

        // No depth buffer for now
        renderPassDesc.depthStencilAttachment = nullptr;

        // We do not use timers for now neither
        renderPassDesc.timestampWriteCount = 0;
        renderPassDesc.timestampWrites = nullptr;

        renderPassDesc.nextInChain = nullptr;

        // Create a render pass. We end it immediately because we use its
        // built-in mechanism for clearing the screen when it begins (see
        // descriptor).
        WGPURenderPassEncoder renderPass =
            wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
        wgpuRenderPassEncoderEnd(renderPass);

        wgpuTextureViewRelease(nextTexture);

        WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
        cmdBufferDescriptor.nextInChain = nullptr;
        cmdBufferDescriptor.label = "Command buffer";
        WGPUCommandBuffer command =
            wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
        wgpuQueueSubmit(queue, 1, &command);

        // We can tell the swap chain to present the next texture.
        wgpuSwapChainPresent(swapChain);
    }

    wgpuSwapChainRelease(swapChain);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance);
    glfwDestroyWindow(window);
    glfwTerminate();

    return;
}

ecstasy::scene ecstasy::app::createScene() {
    // registry_.emplace<>;
    std::cout << "Scene Created! " << std::endl;
}

void ecstasy::app::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
    }
}

ecstasy::app::~app() {
    // wgpuSwapChainRelease(webgpu_swapchain_);
    // wgpuDeviceRelease(webgpu_device_);
    wgpuAdapterRelease(webgpu_adapter_);
    wgpuInstanceRelease(webgpu_instance_);
    glfwDestroyWindow(window_);
    glfwTerminate();
}