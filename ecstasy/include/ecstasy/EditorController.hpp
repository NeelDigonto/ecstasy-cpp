#pragma once
#include <ecstasy/InputController.hpp>

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

  public:
    EditorController() = delete;
    EditorController(const EditorController&) = delete;
    EditorController(EditorController&&) = delete;
    EditorController(InputController* _input_controller,
                     filament::Camera* _camera);
    ~EditorController();

    void animate();
};
} // namespace ecstasy