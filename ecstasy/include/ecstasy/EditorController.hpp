#pragma once
#include <ecstasy/InputController.hpp>
#include <chrono>

namespace filament {
class Camera;
}

namespace ecstasy {
class EditorController {
  private:
    InputController* input_controller_;
    filament::Camera* camera_;
    InputController::SubscriberID cursor_pos_change_sid_;
    InputController::SubscriberID scroll_change_sid_;
    double x_movement_speed{1.0};
    double y_movement_speed{1.0};
    double z_movement_speed{1.0};

    double forwardMovementSpeed = 0.35;
    double backwardMovementSpeed = 0.35;
    double leftMovementSpeed = 0.35;
    double rightMovementSpeed = 0.35;
    double flyUpMovementSpeed = 0.35;
    double flyDownMovementSpeed = 0.35;
    double mouse_wheel_zoom_speed_ = 1.0;
    double horizontalRotationSpeed = 0.001;
    double verticalRotationSpeed = 0.001;

  public:
    EditorController() = delete;
    EditorController(const EditorController&) = delete;
    EditorController(EditorController&&) = delete;
    EditorController(InputController* _input_controller,
                     filament::Camera* _camera);
    ~EditorController();

    void animate(const std::chrono::steady_clock::duration& _delta);
};
} // namespace ecstasy