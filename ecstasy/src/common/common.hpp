#pragma once
#include <cstdint>

#include <common/logger.hpp>
#include <common/timer.hpp>
#include <iostream>
#include <chrono>

#include <Eigen/Dense>

namespace ecstasy {
template <class> inline constexpr bool always_false_v = false;
}

using FilePath = std::string;

using namespace std::literals::chrono_literals;

template <typename T> struct std::hash<Eigen::Vector2<T>> {
    std::size_t operator()(const Eigen::Vector2<T>& _vec2) const {
        return std::hash<T>{}(_vec2.x()) ^ std::hash<T>{}(_vec2.y());
    }
};

template <typename T> struct std::hash<Eigen::Vector3<T>> {
    std::size_t operator()(const Eigen::Vector3<T>& _vec3) const {
        return std::hash<T>{}(_vec3.x()) ^ std::hash<T>{}(_vec3.y()) ^ std::hash<T>{}(_vec3.z());
    }
};

template <typename T> struct std::hash<Eigen::Vector4<T>> {
    std::size_t operator()(const Eigen::Vector4<T>& _vec4) const {
        return std::hash<T>{}(_vec4.x()) ^ std::hash<T>{}(_vec4.y()) ^ std::hash<T>{}(_vec4.z()) ^
               std::hash<T>{}(_vec4.w());
    }
};