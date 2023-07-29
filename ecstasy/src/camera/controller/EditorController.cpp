#include <ecstasy/EditorController.hpp>
#include <ecstasy/ecstasy.hpp>
#include <fmt/core.h>
#include <filament/Camera.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <Eigen/Eigen>
#include <iostream>

ecstasy::EditorController::EditorController(InputController* _input_controller,
                                            filament::Camera* _camera) {
    input_controller_ = _input_controller;
    camera_ = _camera;

    cursor_pos_change_sid_ =
        input_controller_->registerCursorPosChangeUpdater();
    scroll_change_sid_ = input_controller_->registerScrollChangeAccumulator();
}

void ecstasy::EditorController::animate() {

    // if (input_controller_->getKButtonState().at(KButton::W))
    // camera_->setModelMatrix();

    /*  Eigen::Vector3f up_vector{0.0f, 1.0f, 0.0f};
     Eigen::Vector3f camera_position{0.0f, 50.0f, 0.0f};
     Eigen::Vector3f camera_target{0.0f, 0.0f, 0.0f};
     Eigen::Vector3f camera_direction =
         (camera_position - camera_target).normalized();
     Eigen::Vector3f right_axis = up_vector.cross(camera_direction);
     right_axis.normalize();
     Eigen::Matrix4f view; */

    /*     filament::math::mat4 model_mat = camera_->getModelMatrix();

        Eigen::Vector3d camera_position{model_mat[3][0], model_mat[3][1],
                                        model_mat[3][2]};
        Eigen::Vector3f camera_target{0.0f, 0.0f, 0.0f};
        Eigen::Vector3d up_vector{0.0, 1.0, 0.0};

        std::cout << camera_position.format(ecstasy::OctaveFmt) << "\n\n"
                  << std::endl;

        camera_->lookAt(filament::math::float3(0, 50, 0),
                        *reinterpret_cast<filament::math::float3*>(&camera_target),
                        *reinterpret_cast<filament::math::float3*>(&up_vector));
     */

    /*     camera_->lookAt(filament::math::float3(0, 50.5f, 0),
                        filament::math::float3(0, 0, 0),
                        filament::math::float3(1.f, 0, 0)); */

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