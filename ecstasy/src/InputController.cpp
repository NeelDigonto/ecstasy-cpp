#include <ecstasy/InputController.hpp>
#include <cassert>
#include <GLFW/glfw3.h>
#include <ecstasy/ecstasy.hpp>

ecstasy::InputController::InputController(GLFWwindow* _window) {
    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode,
                                   int action, int mods) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        auto input_controller = app->getInputController();
        input_controller->kbutton_state_[key] = action;
    });

    glfwSetCharCallback(_window, [](GLFWwindow* window,
                                    unsigned int codepoint) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
    });

    glfwSetCharModsCallback(_window, [](GLFWwindow* window,
                                        unsigned int codepoint, int mods) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
    });

    glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button,
                                           int action, int mods) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        auto input_controller = app->getInputController();
        input_controller->mbutton_state_[button] = action;
    });

    glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xposIn,
                                         double yposIn) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->current_cursor_pos_ = {xposIn, yposIn};
        app->getInputController()->updateCursorPos({xposIn, yposIn});
    });

    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset,
                                      double yoffset) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->accumulateScrollChange({xoffset, yoffset});
    });
}

decltype(ecstasy::InputController::mbutton_state_)&
ecstasy::InputController::getMButtonState() {
    return mbutton_state_;
}

decltype(ecstasy::InputController::kbutton_state_)&
ecstasy::InputController::getKButtonState() {
    return kbutton_state_;
}

void ecstasy::InputController::updateCursorPos(Eigen::Vector2i&& _new_pos) {
    for (auto& [_, cursor_pos_change] : cursor_pos_changes_)
        cursor_pos_change.current_cursor_pos_ = _new_pos;
}

void ecstasy::InputController::accumulateScrollChange(
    Eigen::Vector2i&& _change) {
    for (auto& [_, scroll_change] : scroll_changes_)
        scroll_change += _change;
}

ecstasy::InputController::SubscriberID
ecstasy::InputController::registerCursorPosChangeUpdater() {
    const auto subscriber_id = cursor_pos_changes_.size();
    cursor_pos_changes_.insert(
        {subscriber_id, {current_cursor_pos_, current_cursor_pos_}});

    return subscriber_id;
}

void ecstasy::InputController::deregisterCursorPosChangeUpdater(
    SubscriberID _subscriber_id) {
    cursor_pos_changes_.erase(_subscriber_id);
}

ecstasy::InputController::SubscriberID
ecstasy::InputController::registerScrollChangeAccumulator() {
    const auto subscriber_id = cursor_pos_changes_.size();
    scroll_changes_.insert({subscriber_id, {0, 0}});

    return subscriber_id;
}

void ecstasy::InputController::deregisterScrollChangeAccumulator(
    SubscriberID _subscriber_id) {
    scroll_changes_.erase(_subscriber_id);
}

ecstasy::CursorPosInfo&
ecstasy::InputController::getCursorPosChange(SubscriberID _subscriber_id) {
    const auto it = cursor_pos_changes_.find(_subscriber_id);
    assert(it == end(cursor_pos_changes_) && "Unkown Subscriber");

    return it->second;
}

void ecstasy::InputController::setCursorPosChange(SubscriberID _subscriber_id,
                                                  Eigen::Vector2i&& _pos) {
    const auto it = cursor_pos_changes_.find(_subscriber_id);
    assert(it != end(cursor_pos_changes_) && "Unkown Subscriber");

    it->second = {_pos, _pos};
}

Eigen::Vector2i&
ecstasy::InputController::getScrollChange(SubscriberID _subscriber_id) {
    const auto it = scroll_changes_.find(_subscriber_id);
    assert(it != end(scroll_change_) && "Unkown Subscriber");

    return it->second;
}

void ecstasy::InputController::setScrollChange(SubscriberID _subscriber_id,
                                               Eigen::Vector2i&& _change) {
    const auto it = scroll_changes_.find(_subscriber_id);
    assert(it == end(scroll_changes_) && "Unkown Subscriber");

    it->second = _change;
}