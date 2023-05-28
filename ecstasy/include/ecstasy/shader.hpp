#pragma once
#include <string>
#include <ecstasy/fd.h>

namespace ecstasy {
class app;

class shader {

  private:
    app* app_;
    WGPUShaderModule shader_module_;

  public:
    shader(const std::string& _shader_source);
    ~shader();
};
} // namespace ecstasy
