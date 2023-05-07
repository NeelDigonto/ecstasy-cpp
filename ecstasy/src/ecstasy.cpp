// #include <SDL.h>
#include <fmt/core.h>
#include <stdio.h>

#include <ecstasy.hpp>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#include <Eigen/Dense>
#include <iostream>

#include <glfw3webgpu.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

using Eigen::MatrixXd;

/* void Ecstasy::display() {
  // SDL2 Hello, World!
  // This should display a white screen for 2 seconds
  // compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
  // run with: ./hello_sdl2

  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return;
  }
  window = SDL_CreateWindow("hello_sdl2", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return;
  }
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL,
               SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();
} */

/* void Ecstasy::compute() {
  MatrixXd m(2, 2);
  m(0, 0) = 3;
  m(1, 0) = 2.5;
  m(0, 1) = -1;
  m(1, 1) = m(1, 0) + m(0, 1);
  std::cout << m << std::endl;
} */

/**
 * Utility function to get a WebGPU adapter, so that
 *     WGPUAdapter adapter = requestAdapter(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
WGPUAdapter requestAdapter(WGPUInstance instance,
                           WGPURequestAdapterOptions const* options) {
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    // Callback called by wgpuInstanceRequestAdapter when the request
    // returns This is a C++ lambda function, but could be any function
    // defined in the global scope. It must be non-capturing (the brackets
    // [] are empty) so that it behaves like a regular C function pointer,
    // which is what wgpuInstanceRequestAdapter expects (WebGPU being a C
    // API). The workaround is to convey what we want to capture through the
    // pUserData pointer, provided as the last argument of
    // wgpuInstanceRequestAdapter and received by the callback as its last
    // argument.
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status,
                                    WGPUAdapter adapter, char const* message,
                                    void* pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
            /*   std::cout << "Adapter Status: " << status << " |
               message: | "
               << message
                        << std::endl; */
        } else {
            std::cout << "Could not get WebGPU adapter: " << message
                      << std::endl;
        }
        userData.requestEnded = true;
    };

    // Call to the WebGPU request adapter procedure
    wgpuInstanceRequestAdapter(instance /* equivalent of navigator.gpu */,
                               options, onAdapterRequestEnded,
                               (void*)&userData);

    // In theory we should wait until onAdapterReady has been called, which
    // could take some time (what the 'await' keyword does in the JavaScript
    // code). In practice, we know that when the
    // wgpuInstanceRequestAdapter() function returns its callback has been
    // called.
    assert(userData.requestEnded);

    return userData.adapter;
}

void Ecstasy::webgpu() {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    // 2. We create the instance using this descriptor
    WGPUInstance instance = wgpuCreateInstance(&desc);

    // 3. We can check whether there is actually an instance created
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return;
    }

    // 4. Display the object (WGPUInstance is a simple pointer, it may be
    // copied around without worrying about its size).
    std::cout << "WGPU instance: " << instance << std::endl;

    WGPURequestAdapterOptions adapterOpts = {};
    WGPUAdapter adapter = requestAdapter(instance, &adapterOpts);
}