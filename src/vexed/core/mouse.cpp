#include "mouse.h"
#include "../../glfw/glfw3.h"

namespace vexed {
    Mouse::Mouse() {
        repeatDelay = 0.5; // Delay before repeat starts
        repeatInterval = 0.025; // Interval for repeat
    }

    void Mouse::initialize() {
        positionX = 0.0f;
        positionY = 0.0f;
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollX = 0.0f;
        scrollY = 0.0f;

        states[ButtonCode::Left] = ButtonState();
        states[ButtonCode::Right] = ButtonState();
        states[ButtonCode::Middle] = ButtonState();
    }

    void Mouse::newFrame() {
        for(auto &k : states) {
            ButtonCode button = k.first;
            int state = k.second.state;

            if(state > 0) {
                if(states[button].down == 0) {
                    states[button].down = 1;
                    states[button].pressed = 0;
                    states[button].lastRepeatTime = glfwGetTime();
                    states[button].repeat = false;
                    if(buttonDown)
                        buttonDown(button);
                } else {
                    states[button].down = 1;
                    states[button].pressed = 1;

                    double currentTime = glfwGetTime();
                    double elapsed = currentTime - states[button].lastRepeatTime;

                    if (!states[button].repeat) {
                        if (elapsed >= repeatDelay) {
                            if(buttonRepeat)
                                buttonRepeat(button);
                            states[button].repeat = true;
                            states[button].lastRepeatTime = currentTime;
                        }
                    } else {
                        if (elapsed >= repeatInterval) {
                            if(buttonRepeat)
                                buttonRepeat(button);
                            states[button].lastRepeatTime = currentTime;
                        }
                    }

                    if(buttonPress)
                        buttonPress(button);
                }

                states[button].up = 0;
            } else {
                if(states[button].down == 1 || states[button].pressed == 1) {
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 1;
                    if(buttonUp)
                        buttonUp(button);
                } else {
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 0;
                }
            }
        }
    }

    void Mouse::endFrame() {
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollX = 0.0f;
        scrollY = 0.0f;
    }

    void Mouse::setPosition(float x, float y) {
        float prevX = positionX;
        float prevY = positionY;

        positionX = x;
        positionY = y;

        deltaX = x - prevX;
        deltaY = y - prevY;
    }

    void Mouse::setWindowPosition(float x, float y) {
        windowPositionX = x;
        windowPositionY = y;
    }

    void Mouse::setScrollDirection(float x, float y) {
        scrollX = x;
        scrollY = y;
    }

    void Mouse::setState(ButtonCode buttoncode, int32_t state) {
        states[buttoncode].state = state;
    }

    bool Mouse::getButton(ButtonCode buttoncode) {
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 1;
    }

    bool Mouse::getButtonDown(ButtonCode buttoncode) {
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 0;
    }

    bool Mouse::getButtonUp(ButtonCode buttoncode) {
        ButtonState state = states[buttoncode];
        return state.up > 0;
    }

    float Mouse::getX() const {
        return positionX;
    }

    float Mouse::getY() const {
        return positionY;
    }

    float Mouse::getAbsoluteX() const {
        return windowPositionX + positionX;
    }

    float Mouse::getAbsoluteY() const {
        return windowPositionY + positionY;
    }

    float Mouse::getDeltaX() const {
        return deltaX;
    }

    float Mouse::getDeltaY() const {
        return deltaY;
    }

    float Mouse::getScrollX() const {
        return scrollX;
    }

    float Mouse::getScrollY() const {
        return scrollY;
    }
}