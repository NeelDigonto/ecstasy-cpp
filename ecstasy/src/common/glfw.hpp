#pragma once

// GLFW_EXPOSE_NATIVE_WAYLAND GLFW_EXPOSE_NATIVE_X11
// #define GLFW_EXPOSE_NATIVE_WAYLAND

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef linux
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <GLFW/glfw3.h>
// #define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>
