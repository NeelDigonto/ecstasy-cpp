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

#include <numbers>

template <typename T> inline T degreeToRad(T _degree) { return _degree * (std::numbers::pi / 180.0); }

template <typename T> inline Eigen::Vector3<T> degreeToRad(Eigen::Vector2<T> _degree) {
    return Eigen::Vector2<T>{degreeToRad(_degree.x()), degreeToRad(_degree.y())};
}

template <typename T> inline Eigen::Vector3<T> degreeToRad(Eigen::Vector3<T> _degree) {
    return Eigen::Vector3<T>{degreeToRad(_degree.x()), degreeToRad(_degree.y()), degreeToRad(_degree.z())};
}

template <typename T> inline Eigen::Vector3<T> degreeToRad(Eigen::Vector4<T> _degree) {
    return Eigen::Vector4<T>{degreeToRad(_degree.x()), degreeToRad(_degree.y()), degreeToRad(_degree.z()),
                             degreeToRad(_degree.w())};
}

template <typename T> inline T radToDegree(T _radian) { return _radian * (180.0 / std::numbers::pi); }

template <typename T, typename U>
inline auto createTransform(const Eigen::Vector3<T>& _translation, const Eigen::Vector3<U>& _rotation) {
    Eigen::Quaternionf q = Eigen::AngleAxisf(_rotation.x(), Eigen::Vector3f::UnitX()) *
                           Eigen::AngleAxisf(_rotation.y(), Eigen::Vector3f::UnitY()) *
                           Eigen::AngleAxisf(_rotation.z(), Eigen::Vector3f::UnitZ());
    q.normalize();

    Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();
    transform.block<3, 3>(0, 0) = q.toRotationMatrix();
    transform.block<3, 1>(0, 3) = _translation;

    return transform;
}
