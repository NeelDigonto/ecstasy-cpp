#pragma once

namespace ecstasy {
namespace scene {
class scene {
    virtual void build() = 0;
    virtual void animate() = 0;
    virtual void destroy() = 0;
};
} // namespace scene
} // namespace ecstasy