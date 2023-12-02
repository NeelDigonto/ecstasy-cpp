#include <controller/EditorController.hpp>
#include <ecstasy/ecstasy.hpp>
#include <fmt/core.h>
#include <filament/Camera.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <math/TQuatHelpers.h>
#include <math/TMatHelpers.h>
#include <Eigen/Eigen>

#include <common/common.hpp>

ecstasy::EditorController::EditorController(InputController* _input_controller, filament::Camera* _camera) {
    input_controller_ = _input_controller;
    camera_ = _camera;

    camera_position_ = {0., 0., 60.};
    camera_rotation_ = Eigen::AngleAxisd(0., Eigen::Vector3d::UnitX()) *
                       Eigen::AngleAxisd(0., Eigen::Vector3d::UnitY()) *
                       Eigen::AngleAxisd(0., Eigen::Vector3d::UnitZ());
    camera_rotation_.normalize();

    auto viewport_dimension = input_controller_->getViewportDimension();
    camera_->setProjection(
        45.0, static_cast<double>(viewport_dimension.x()) / static_cast<double>(viewport_dimension.y()), 0.1,
        1000, filament::Camera::Fov::VERTICAL);

    camera_->setModelMatrix(filament::math::mat4(
        filament::math::mat3(filament::math::quat{camera_rotation_.x(), camera_rotation_.y(),
                                                  camera_rotation_.z(), camera_rotation_.w()}),
        filament::math::double3{camera_position_.x(), camera_position_.y(), camera_position_.z()}));

    x_movement_speed_ = 1.0E-4;
    y_movement_speed_ = 2.0E-5;
    z_movement_speed_ = 1.0E-4;
    mouse_wheel_zoom_speed_ = 2.0E-2;
    horizontalRotationSpeed = 1.0E-3;
    verticalRotationSpeed = 1.0E-3;
}

void ecstasy::EditorController::animate(const std::chrono::steady_clock::duration& _delta) {
    double delta = std::chrono::duration_cast<std::chrono::microseconds>(_delta).count();
    Eigen::Vector3d translation{0., 0., 0.};

    if (input_controller_->getKButtonState().at(KButton::A))
        translation.x() = -delta * x_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::D))
        translation.x() = delta * x_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::SPACE))
        translation.y() = delta * y_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::LEFT_CONTROL))
        translation.y() = -delta * y_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::W))
        translation.z() = -delta * z_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::S))
        translation.z() = delta * z_movement_speed_;

    const auto mouseMove = input_controller_->getScrollChange();
    if (mouseMove.x() != 0 || mouseMove.y() != 0) {
        input_controller_->setScrollChange({0., 0.});

        translation.z() = mouse_wheel_zoom_speed_ * -mouseMove.y();
    }

    camera_position_ += translation;

    auto cursor_pos_change = input_controller_->getCursorPosChange();
    if (std::abs(cursor_pos_change.x()) > std::numeric_limits<double>::epsilon() ||
        std::abs(cursor_pos_change.y()) > std::numeric_limits<double>::epsilon()) {
        /* log::info("{}, {}\n", cursor_pos_change.x() * verticalRotationSpeed,
                  cursor_pos_change.y() * horizontalRotationSpeed); */

        Eigen::Quaterniond rotation;
        /* rotation = Eigen::AngleAxisd(cursor_pos_change.x(), Eigen::Vector3d::UnitX()) *
                   Eigen::AngleAxisd(cursor_pos_change.y(), Eigen::Vector3d::UnitY()) *
                   Eigen::AngleAxisd(cursor_pos_change.z(), Eigen::Vector3d::UnitZ()); */

        rotation =
            /*     Eigen::AngleAxisd(cursor_pos_change.y() * horizontalRotationSpeed,
               Eigen::Vector3d::UnitX()) * */
            Eigen::AngleAxisd(cursor_pos_change.x() * verticalRotationSpeed, Eigen::Vector3d::UnitY());
        rotation.normalize();

        camera_rotation_ = camera_rotation_ * rotation;
        camera_rotation_.normalize();

        auto euler = camera_rotation_.toRotationMatrix().eulerAngles(0, 1, 2);
        // log::info("{}, {}, {}\n", euler.x(), euler.y(), euler.z());

        input_controller_->setCursorPosChange({0., 0.});
    }

    camera_->setModelMatrix(filament::math::mat4(
        filament::math::mat3(filament::math::quat{camera_rotation_.w(), camera_rotation_.x(),
                                                  camera_rotation_.y(), camera_rotation_.z()}),
        filament::math::double3{camera_position_.x(), camera_position_.y(), camera_position_.z()}));
}

ecstasy::EditorController::~EditorController() {}