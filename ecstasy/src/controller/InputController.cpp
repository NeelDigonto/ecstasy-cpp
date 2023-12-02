#include <controller/InputController.hpp>
#include <cassert>
#include <GLFW/glfw3.h>
#include <common/common.hpp>
#include <ecstasy/ecstasy.hpp>

ecstasy::InputController::InputController(GLFWwindow* _window, Eigen::Vector2i _window_dimension) {

    /*  #define GLFW_RELEASE 0
        #define GLFW_PRESS 1
        #define GLFW_REPEAT 2
    */

    mbutton_state_.fill(GLFW_RELEASE);
    kbutton_state_.fill(GLFW_RELEASE);

    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    viewport_dimension_ = {width, height};

    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* window, int width, int height) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->setViewportDimension({width, height});
    });

    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key <= KButton::UNKNOWN && key > KButton::LAST) {
            return;
        }

        if (key == KButton::ESCAPE) {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        auto input_controller = app->getInputController();
        input_controller->kbutton_state_[key] = action;
    });

    // glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int codepoint) {
    //     auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
    // });

    // glfwSetCharModsCallback(_window, [](GLFWwindow* window, unsigned int codepoint, int mods) {
    //     auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
    // });

    glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        auto input_controller = app->getInputController();
        input_controller->mbutton_state_[button] = action;
    });

    glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xposIn, double yposIn) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->setCursorPos(
            {xposIn, app->getInputController()->getViewportDimension().y() - yposIn});
    });

    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto app = static_cast<ecstasy::app*>(glfwGetWindowUserPointer(window));
        app->getInputController()->setScrollChange({xoffset, yoffset});
    });
}

void ecstasy::InputController::setViewportDimension(const Eigen::Vector2i& _viewport_dimension) {
    viewport_dimension_ = _viewport_dimension;
}

const Eigen::Vector2i& ecstasy::InputController::getViewportDimension() const { return viewport_dimension_; }

decltype(ecstasy::InputController::mbutton_state_)& ecstasy::InputController::getMButtonState() {
    return mbutton_state_;
}

decltype(ecstasy::InputController::kbutton_state_)& ecstasy::InputController::getKButtonState() {
    return kbutton_state_;
}

void ecstasy::InputController::setCursorPos(const Eigen::Vector2d& _new_pos) {

    const auto diffx = current_cursor_pos_.x() - _new_pos.x();
    const auto diffy = current_cursor_pos_.y() - _new_pos.y();

    cursor_pos_change_ = {diffx, diffy};
    current_cursor_pos_ = _new_pos;

    // log::info("{}, {}\n", cursor_pos_change_.x(), cursor_pos_change_.y());
}

void ecstasy::InputController::setCursorPosChange(const Eigen::Vector2d& _cursor_pos_change) {
    cursor_pos_change_ = _cursor_pos_change;
}
void ecstasy::InputController::setScrollChange(const Eigen::Vector2d& _change) { scroll_change_ = _change; }

const Eigen::Vector2d& ecstasy::InputController::getCursorPosChange() const { return cursor_pos_change_; }
const Eigen::Vector2d& ecstasy::InputController::getScrollChange() const { return scroll_change_; }
