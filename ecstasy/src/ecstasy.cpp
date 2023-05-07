#include <SDL.h>
#include <fmt/core.h>
#include <stdio.h>

#include <ecstasy.hpp>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#include <webgpu/webgpu.h>

#include <Eigen/Dense>
#include <iostream>

using Eigen::MatrixXd;

void Ecstasy::display() {
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
}

void Ecstasy::compute() {
  MatrixXd m(2, 2);
  m(0, 0) = 3;
  m(1, 0) = 2.5;
  m(0, 1) = -1;
  m(1, 1) = m(1, 0) + m(0, 1);
  std::cout << m << std::endl;
}

void Ecstasy::dawnTest() {
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
}