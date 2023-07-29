#include <ecstasy/EditorController.hpp>
#include <fmt/core.h>

ecstasy::EditorController::EditorController(InputController* _input_controller,
                                            filament::Camera* _camera) {
    input_controller_ = _input_controller;
    camera_ = _camera;

    cursor_pos_change_sid_ =
        input_controller_->registerCursorPosChangeUpdater();
    scroll_change_sid_ = input_controller_->registerScrollChangeAccumulator();
}

void ecstasy::EditorController::animate() {
    auto cursor_pos_info =
        input_controller_->getCursorPosChange(cursor_pos_change_sid_);
    const auto cursor_pos_diff = cursor_pos_info.current_cursor_pos_ -
                                 cursor_pos_info.last_animate_cursor_pos;

    if (cursor_pos_diff.x() != 0 || cursor_pos_diff.y() != 0)
        fmt::print("{}, {}\n", cursor_pos_diff.x(), cursor_pos_diff.y());
    input_controller_->setCursorPosChange(
        cursor_pos_change_sid_, std::move(cursor_pos_info.current_cursor_pos_));

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