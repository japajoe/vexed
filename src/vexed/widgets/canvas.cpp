#include "canvas.h"
#include <stack>

namespace vexed {
    Canvas::Canvas() : Widget() {
        setPosition(Vector2(0, 0));
        setSize(Vector2(0, 0));
    }

    void Canvas::initialize() {
        auto application = Application::getInstance();
        auto mouse = application->getMouse();
        auto keyboard = application->getKeyboard();

        mouse->buttonDown = [this] (ButtonCode b) { onButtonDownCallback(b); };
        mouse->buttonUp = [this] (ButtonCode b) { onButtonUpCallback(b); };
        mouse->buttonPress = [this] (ButtonCode b) { onButtonPressCallback(b); };
        mouse->buttonRepeat = [this] (ButtonCode b) { onButtonRepeatCallback(b); };

        keyboard->keyDown = [this] (KeyCode k) { onKeyDownCallback(k); };
        keyboard->keyUp = [this] (KeyCode k) { onKeyUpCallback(k); };
        keyboard->keyPress = [this] (KeyCode k) { onKeyPressCallback(k); };
        keyboard->charPress = [this] (uint32_t c) { onCharPressCallback(c); };
        keyboard->keyRepeat = [this] (KeyCode k) { onKeyRepeatCallback(k); };
    }

    void Canvas::update() {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            checkEvents(currentControl);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::render() {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            currentControl->onRender();

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::checkEvents(Widget *widget) {
        if(!widget)
            return;
        if(widget == this)
            return;

        Mouse *mouse = Application::getInstance()->getMouse();
        Vector2 mousePosition(mouse->getX(), mouse->getY());

        bool hovered = widget->containsPoint(mousePosition);
        bool hoveredPreviousFrame = widget->state & WidgetState_Hovered;

        if(hovered) {
            if(!hoveredPreviousFrame) {
                widget->onMouseEnter();
            }
        } else {
            if(hoveredPreviousFrame) {
                widget->onMouseLeave();
            }
        }
    }

    void Canvas::onRender() {

    }

    void Canvas::onCharPressCallback(uint32_t codepoint) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onCharPress(codepoint);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onKeyDownCallback(KeyCode keycode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onKeyDown(keycode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onKeyUpCallback(KeyCode keycode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onKeyUp(keycode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onKeyPressCallback(KeyCode keycode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onKeyPress(keycode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onKeyRepeatCallback(KeyCode keycode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onKeyRepeat(keycode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onButtonDownCallback(ButtonCode buttoncode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onButtonDown(buttoncode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onButtonUpCallback(ButtonCode buttoncode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onButtonUp(buttoncode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onButtonPressCallback(ButtonCode buttoncode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onButtonPress(buttoncode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }

    void Canvas::onButtonRepeatCallback(ButtonCode buttoncode) {
        size_t stackIndex = 0;
        stack[stackIndex++] = this;

        while (stackIndex > 0) {
            Widget* currentControl = stack[--stackIndex];

            if(currentControl != this)
                currentControl->onButtonRepeat(buttoncode);

            for (auto it = currentControl->children.rbegin(); it != currentControl->children.rend(); ++it) {
                if (stackIndex < WORKING_STATIC_SIZE) {
                    stack[stackIndex++] = it->get();
                }
            }
        }
    }
}