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

    x_movement_speed_ = 1.0E-4;
    y_movement_speed_ = 2.0E-5;
    z_movement_speed_ = 1.0E-4;
    mouse_wheel_zoom_speed_ = 5.0E-2;
    horizontalRotationSpeed = 1.0E-3;
    verticalRotationSpeed = 1.0E-3;

    auto viewport_dimension = input_controller_->getViewportDimension();
    camera_->setProjection(
        45.0, static_cast<double>(viewport_dimension.x()) / static_cast<double>(viewport_dimension.y()), 0.1,
        100, filament::Camera::Fov::VERTICAL);

    /*    camera_position_ = {0., 0., 60.};
       Eigen::Quaterniond camera_rotationq = Eigen::AngleAxisd(0., Eigen::Vector3d::UnitX()) *
                                             Eigen::AngleAxisd(0., Eigen::Vector3d::UnitY()) *
                                             Eigen::AngleAxisd(0., Eigen::Vector3d::UnitZ());
       camera_rotationq.normalize();

       camera_->setModelMatrix(filament::math::mat4(
           filament::math::mat3(filament::math::quat{camera_rotationq.w(), camera_rotationq.x(),
                                                     camera_rotationq.y(), camera_rotationq.z()}),
           filament::math::double3{camera_position_.x(), camera_position_.y(), camera_position_.z()})); */

    Eigen::Matrix3d R = Eigen::Quaterniond(Eigen::AngleAxisd(0., Eigen::Vector3d::UnitX()) *
                                           Eigen::AngleAxisd(0., Eigen::Vector3d::UnitY()) *
                                           Eigen::AngleAxisd(0., Eigen::Vector3d::UnitZ()))
                            .toRotationMatrix();
    Eigen::Vector3d T{0., 0., 60.};
    camera_model_transformation_.setIdentity(); // Set to Identity to make bottom row of Matrix 0,0,0,1
    camera_model_transformation_.block<3, 3>(0, 0) = R;
    camera_model_transformation_.block<3, 1>(0, 3) = T;

    camera_->setModelMatrix(
        filament::math::mat4(*reinterpret_cast<filament::math::mat4*>(&camera_model_transformation_)));
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

    Eigen::Quaterniond rotation_quat = Eigen::AngleAxisd(0., Eigen::Vector3d::UnitX()) *
                                       Eigen::AngleAxisd(0., Eigen::Vector3d::UnitY()) *
                                       Eigen::AngleAxisd(0., Eigen::Vector3d::UnitZ());

    auto cursor_pos_change = input_controller_->getCursorPosChange();
    if (std::abs(cursor_pos_change.x()) > std::numeric_limits<double>::epsilon() ||
        std::abs(cursor_pos_change.y()) > std::numeric_limits<double>::epsilon()) {

        rotation_quat =
            Eigen::AngleAxisd(-cursor_pos_change.y() * verticalRotationSpeed, Eigen::Vector3d::UnitX()) *
            Eigen::AngleAxisd(cursor_pos_change.x() * horizontalRotationSpeed, Eigen::Vector3d::UnitY());
        rotation_quat.normalize();

        input_controller_->setCursorPosChange({0., 0.});
    }

    Eigen::Matrix4d current_frame_transformation;

    current_frame_transformation.setIdentity(); // Set to Identity to make bottom row of Matrix 0,0,0,1
    current_frame_transformation.block<3, 3>(0, 0) = rotation_quat.toRotationMatrix();
    current_frame_transformation.block<3, 1>(0, 3) = translation;

    camera_model_transformation_ = camera_model_transformation_ * current_frame_transformation;

    camera_->setModelMatrix(
        filament::math::mat4(*reinterpret_cast<filament::math::mat4*>(&camera_model_transformation_)));
}

ecstasy::EditorController::~EditorController() {}
