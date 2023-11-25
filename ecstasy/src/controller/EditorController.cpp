#include <controller/EditorController.hpp>
#include <ecstasy/ecstasy.hpp>
#include <fmt/core.h>
#include <filament/Camera.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <Eigen/Eigen>
#include <iostream>

ecstasy::EditorController::EditorController(InputController* _input_controller, filament::Camera* _camera) {
    input_controller_ = _input_controller;
    camera_ = _camera;

    cursor_pos_change_sid_ = input_controller_->registerCursorPosChangeUpdater();
    scroll_change_sid_ = input_controller_->registerScrollChangeAccumulator();
}

void ecstasy::EditorController::animate(const std::chrono::steady_clock::duration& _delta) {

    filament::math::double3 up_vector{0.0, 1.0, 0.0};
    filament::math::mat4 model_mat = camera_->getModelMatrix();
    filament::math::double3 camera_position{model_mat[3][0], model_mat[3][1], model_mat[3][2]};
    auto camera_target_direction = normalize(cross(camera_->getLeftVector(), camera_->getUpVector()));
    auto camera_target = camera_position - camera_target_direction;

    // Update
    double delta = std::chrono::duration_cast<std::chrono::nanoseconds>(_delta).count();
    filament::math::double3 translation{0., 0., 0.};

    const auto scroll_change = input_controller_->getScrollChange(scroll_change_sid_);

    if (scroll_change.y() != 0) {
        translation.z = -scroll_change.y() * mouse_wheel_zoom_speed_;
        input_controller_->setScrollChange(scroll_change_sid_, {0, 0});
        /* fmt::print("translation: {} {} {}\n", translation.x, translation.y,
                   translation.z); */
    }

    if (input_controller_->getKButtonState().at(KButton::W))
        translation.z = -delta * z_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::S))
        translation.z = +delta * z_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::A))
        translation.x = -delta * x_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::D))
        translation.x = delta * x_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::SPACE))
        translation.y = delta * y_movement_speed_;

    if (input_controller_->getKButtonState().at(KButton::LEFT_CONTROL))
        translation.y = -delta * y_movement_speed_;

    /*  fmt::print("translation: {} {} {}\n", translation.x, translation.y,
                translation.z); */

    /*  Eigen::Vector3f up_vector{0.0f, 1.0f, 0.0f};
    Eigen::Vector3f camera_position{0.0f, 50.0f, 0.0f};
    Eigen::Vector3f camera_target{0.0f, 0.0f, 0.0f};
    Eigen::Vector3f camera_direction =
        (camera_position - camera_target).normalized();
    Eigen::Vector3f right_axis = up_vector.cross(camera_direction);
    right_axis.normalize();
    Eigen::Matrix4f view; */

    /*     fmt::print("camera_position: {} {} {}\n", camera_position.x,
                camera_position.y, camera_position.z);
     fmt::print("camera_target_direction: {} {} {}\n",
    camera_target_direction.x, camera_target_direction.y,
    camera_target_direction.z); */

    camera_position += translation;
    camera_target += translation;

    camera_->lookAt(camera_position, camera_target, up_vector);

    /*  auto cursor_pos_info =
        input_controller_->getCursorPosChange(cursor_pos_change_sid_);
    if (cursor_pos_info.cursor_pos_diff_.x() != 0 ||
            cursor_pos_info.cursor_pos_diff_.y() != 0)
            fmt::print("{}, {}\n", cursor_pos_info.cursor_pos_diff_.x(),
                       cursor_pos_info.cursor_pos_diff_.y());
        input_controller_->setCursorPosChange(
            cursor_pos_change_sid_,
       std::move(cursor_pos_info.current_cursor_pos_)); */

    /* const auto mouseMove =
        input_controller_->getScrollChange(scroll_change_sid_);
    if (mouseMove.x() != 0 || mouseMove.y() != 0)
        fmt::print("{}, {}\n", mouseMove.x(), mouseMove.y());
    input_controller_->setScrollChange(scroll_change_sid_,
                                       std::move<Eigen::Vector2i>({0, 0})); */
}

ecstasy::EditorController::~EditorController() {
    input_controller_->deregisterCursorPosChangeUpdater(cursor_pos_change_sid_);
    input_controller_->deregisterScrollChangeAccumulator(scroll_change_sid_);
}