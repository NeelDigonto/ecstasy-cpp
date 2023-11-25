#pragma once
#include <chrono>

namespace ecstasy {
class timer {
  private:
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;

  public:
    inline timer() noexcept : start_{std::chrono::steady_clock::now()} {};
    inline void start() noexcept { start_ = std::chrono::steady_clock::now(); }
    inline std::chrono::microseconds measure() const noexcept {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() -
                                                                     start_);
    }
};
} // namespace ecstasy

// #define INIT_PROFILER timer timer;
#define PROFILE(x) ecstasy::timer timer_##x;
#define MEASURE(x) timer_##x.measure()