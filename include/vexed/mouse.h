#ifndef VEXED_MOUSE_H
#define VEXED_MOUSE_H

#include <cstdint>
#include <unordered_map>

namespace vexed {
    #define GLFW__MOUSE_BUTTON_1         0
    #define GLFW__MOUSE_BUTTON_2         1
    #define GLFW__MOUSE_BUTTON_3         2
    #define GLFW__MOUSE_BUTTON_4         3
    #define GLFW__MOUSE_BUTTON_5         4
    #define GLFW__MOUSE_BUTTON_6         5
    #define GLFW__MOUSE_BUTTON_7         6
    #define GLFW__MOUSE_BUTTON_8         7
    #define GLFW__MOUSE_BUTTON_LAST      GLFW__MOUSE_BUTTON_8
    #define GLFW__MOUSE_BUTTON_LEFT      GLFW__MOUSE_BUTTON_1
    #define GLFW__MOUSE_BUTTON_RIGHT     GLFW__MOUSE_BUTTON_2
    #define GLFW__MOUSE_BUTTON_MIDDLE    GLFW__MOUSE_BUTTON_3

    enum class ButtonCode {
        Unknown = -1,
        Left = GLFW__MOUSE_BUTTON_LEFT,
        Right = GLFW__MOUSE_BUTTON_RIGHT,
        Middle = GLFW__MOUSE_BUTTON_MIDDLE
    };

    struct ButtonState {
        int down;
        int up;
        int pressed;
        int state;

        ButtonState() {
            down = 0;
            up = 0;
            pressed = 0;
            state = 0;
        }
    };

    class Mouse {
    public:
        Mouse();
        void initialize();
        void newFrame();
        void endFrame();
        void setPosition(float x, float y);
        void setScrollDirection(float x, float y);
        void setState(ButtonCode buttoncode, int32_t state);
        bool getButton(ButtonCode buttoncode);
        bool getButtonDown(ButtonCode buttoncode);
        bool getButtonUp(ButtonCode buttoncode);
        float getX() const;
        float getY() const;
        float getDeltaX() const;
        float getDeltaY() const;
        float getScrollX() const;
        float getScrollY() const;
    private:
        std::unordered_map<ButtonCode,ButtonState> states;
        float positionX;
        float positionY;
        float deltaX;
        float deltaY;
        float scrollX;
        float scrollY;
    };
}

#endif