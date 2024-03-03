#pragma once
#include <Eigen/Dense>

template <typename T> static inline Eigen::Quaternionf getQuatFromEuler(Eigen::Vector3<T> _euler) {
    Eigen::Quaternionf q;
    q = Eigen::AngleAxis<T>(_euler.x(), Eigen::Vector3<T>::UnitX()) *
        Eigen::AngleAxis<T>(_euler.y(), Eigen::Vector3<T>::UnitY()) *
        Eigen::AngleAxis<T>(_euler.z(), Eigen::Vector3<T>::UnitZ());
    q.normalize();

    return q;
}

static inline auto getFloat4FromEuler(Eigen::Vector3f _euler) -> Eigen::Vector4f {
    auto q = getQuatFromEuler<float>(_euler);
    return Eigen::Vector4f{q.x(), q.y(), q.z(), q.w()};
}