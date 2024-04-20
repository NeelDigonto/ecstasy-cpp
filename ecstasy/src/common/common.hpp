#pragma once
#include <cstdint>

#include <common/logger.hpp>
#include <common/timer.hpp>
#include <iostream>
#include <chrono>

namespace ecstasy {
template <class> inline constexpr bool always_false_v = false;
}

using FilePath = std::string;

using namespace std::literals::chrono_literals;
