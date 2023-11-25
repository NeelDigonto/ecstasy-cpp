#pragma once
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>

namespace ecstasy {
class log {
  private:
    log() = delete;
    log(const log&) = delete;
    log(log&&) = delete;

  public:
    static void init() { log::info("Logger Initialized."); }

    template <typename... Args> static void trace(std::string_view _sv, Args&&... args) {
        fmt::print(fg(fmt::color::gray), "TRACE : {}\n",
                   fmt::format(fg(fmt::color::gray), _sv, std::forward<Args>(args)...));
    }

    template <typename... Args> static void debug(std::string_view _sv, Args&&... args) {
        if constexpr (!(NDEBUG))
            fmt::print(fg(fmt::color::blue), "DEBUG  : {}\n",
                       fmt::format(fg(fmt::color::blue), _sv, std::forward<Args>(args)...));
    }

    template <typename... Args> static void info(std::string_view _sv, Args&&... args) {
        fmt::print(fg(fmt::color::white), "INFO  : {}\n",
                   fmt::format(fg(fmt::color::white), _sv, std::forward<Args>(args)...));
    }

    template <typename... Args> static void warn(std::string_view _sv, Args&&... args) {
        fmt::print(fg(fmt::color::yellow), "WARN  : {}\n",
                   fmt::format(fg(fmt::color::yellow), _sv, std::forward<Args>(args)...));
    }

    template <typename... Args> static void error(std::string_view _sv, Args&&... args) {
        fmt::print(fg(fmt::color::orange), "ERROR : {}\n",
                   fmt::format(fg(fmt::color::orange), _sv, std::forward<Args>(args)...));
    }

    template <typename... Args> static void fatal(std::string_view _sv, Args&&... args) {
        fmt::print(fg(fmt::color::red), "FATAL : {}\n",
                   fmt::format(fg(fmt::color::red), _sv, std::forward<Args>(args)...));
    }
};
} // namespace ecstasy