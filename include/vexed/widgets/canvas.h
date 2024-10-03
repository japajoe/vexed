#ifndef VEXED_CANVAS_H
#define VEXED_CANVAS_H

#include "widget.h"

namespace vexed {
    constexpr size_t WORKING_STATIC_SIZE = 1024;

    class Canvas : public Widget {
    public:
        Canvas();
        void initialize();
        void update();
        void render();
    protected:
        void onRender() override;
    private:
        Widget* stack[WORKING_STATIC_SIZE];
        void checkEvents(Widget *widget);
        void onCharPressCallback(uint32_t codepoint);
        void onKeyDownCallback(KeyCode keycode);
        void onKeyUpCallback(KeyCode keycode);
        void onKeyPressCallback(KeyCode keycode);
        void onKeyRepeatCallback(KeyCode keycode);
        void onButtonDownCallback(ButtonCode buttoncode);
        void onButtonUpCallback(ButtonCode buttoncode);
        void onButtonPressCallback(ButtonCode buttoncode);
        void onButtonRepeatCallback(ButtonCode buttoncode);
    };
}

#endif