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

    auto viewport_dimension = input_controller_->viewport_dimension_;
    camera_->setProjection(
        45.0, static_cast<double>(viewport_dimension.x()) / static_cast<double>(viewport_dimension.y()), 0.1,
        50, filament::Camera::Fov::VERTICAL);

    camera_->setModelMatrix(filament::math::mat4(
        filament::math::mat3(filament::math::quat{camera_rotation_.x(), camera_rotation_.y(),
                                                  camera_rotation_.z(), camera_rotation_.w()}),
        filament::math::double3{camera_position_.x(), camera_position_.y(), camera_position_.z()}));

    x_movement_speed_ = 1.0E-4;
    y_movement_speed_ = 1.0E-4;
    z_movement_speed_ = 1.0E-4;
    mouse_wheel_zoom_speed_ = 5.0E-2;
    horizontalRotationSpeed = 1.0E-3;
    verticalRotationSpeed = 1.0E-3;

    cursor_pos_change_sid_ = input_controller_->registerCursorPosChangeUpdater();
    scroll_change_sid_ = input_controller_->registerScrollChangeAccumulator();
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

    const auto mouseMove = input_controller_->getScrollChange(scroll_change_sid_);
    if (mouseMove.x() != 0 || mouseMove.y() != 0) {
        input_controller_->setScrollChange(scroll_change_sid_, std::move<Eigen::Vector2i>({0, 0}));

        translation.z() = mouse_wheel_zoom_speed_ * -mouseMove.y();
    }

    camera_position_ += translation;

    // log::info("{} {} {}", camera_position_.x(), camera_position_.y(), camera_position_.z());

    auto cursor_pos_change = input_controller_->getCursorPosChange(cursor_pos_change_sid_);
    if (cursor_pos_change.x() != 0 || cursor_pos_change.y() != 0) {
        log::info("{}, {}\n", cursor_pos_change.x(), cursor_pos_change.y());
        input_controller_->setCursorPosChange(cursor_pos_change_sid_, std::move<Eigen::Vector2i>({0, 0}));
    }

    camera_->setModelMatrix(filament::math::mat4(
        filament::math::mat3(filament::math::quat{camera_rotation_.x(), camera_rotation_.y(),
                                                  camera_rotation_.z(), camera_rotation_.w()}),
        filament::math::double3{camera_position_.x(), camera_position_.y(), camera_position_.z()}));
}

ecstasy::EditorController::~EditorController() {
    input_controller_->deregisterCursorPosChangeUpdater(cursor_pos_change_sid_);
    input_controller_->deregisterScrollChangeAccumulator(scroll_change_sid_);
}