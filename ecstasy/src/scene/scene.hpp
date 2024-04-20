#pragma once
#include <chrono>

#include <material/MaterialManager.hpp>

namespace ecstasy {
namespace scene {
class scene {

  public:
    virtual void build() = 0;
    virtual void animate(std::chrono::steady_clock::duration _last_animation_time) = 0;
    virtual void destroy() = 0;
};
} // namespace scene
} // namespace ecstasy