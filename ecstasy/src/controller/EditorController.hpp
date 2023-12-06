#pragma once
#include <controller/InputController.hpp>
#include <chrono>

namespace filament {
class Camera;
}

namespace ecstasy {
class EditorController {
  private:
    InputController* input_controller_;
    filament::Camera* camera_;
    double x_movement_speed_;
    double y_movement_speed_;
    double z_movement_speed_;
    double mouse_wheel_zoom_speed_;
    double horizontalRotationSpeed;
    double verticalRotationSpeed;

    Eigen::Matrix4d camera_model_transformation_;
    Eigen::Vector3d up_vector_{0., 1., 0.};

  public:
    EditorController() = delete;
    EditorController(const EditorController&) = delete;
    EditorController(EditorController&&) = delete;
    EditorController(InputController* _input_controller, filament::Camera* _camera/* ,
                     Eigen::Vector3d _camera_position, Eigen::Vector3d _camera_target */);
    ~EditorController();

    void animate(const std::chrono::steady_clock::duration& _delta);
};
} // namespace ecstasy