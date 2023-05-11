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

ecstasy::app::app() {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    webgpu_instance_ = wgpuCreateInstance(&desc);
    if (!webgpu_instance_) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return;
    }

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW!" << std::endl;
        return;
    }

    // We ask GLFW not to set up any graphics API, we'll do it manually
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window_ = glfwCreateWindow(1280, 720, "Ecstasy", NULL, NULL);
    if (!window_) {
        std::cerr << "Could not open window!" << std::endl;
        return;
    }

    std::cout << "Requesting adapter..." << std::endl;

    // Utility function provided by glfw3webgpu.h
    WGPUSurface surface = glfwGetWGPUSurface(webgpu_instance_, window_);

    // Adapter options: we need the adapter to draw to the window's surface
    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;

    // Get the adapter, see the comments in the definition of the body of the
    // requestAdapter function above.
    webgpu_adapter_ = requestAdapter(webgpu_instance_, &adapterOpts);

    std::cout << "Got adapter: " << webgpu_adapter_ << std::endl;

    inspectAdapter(webgpu_adapter_);

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