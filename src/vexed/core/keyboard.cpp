#include "keyboard.h"
#include "../../glfw/glfw3.h"

namespace vexed {
    Keyboard::Keyboard() {
        repeatDelay = 0.5; // Delay before repeat starts
        repeatInterval = 0.025; // Interval for repeat
    }

    void Keyboard::initialize() {
        states[KeyCode::Unknown] = KeyState();
        states[KeyCode::Space] = KeyState();
        states[KeyCode::Apostrophe] = KeyState();
        states[KeyCode::Comma] = KeyState();
        states[KeyCode::Minus] = KeyState();
        states[KeyCode::Period] = KeyState();
        states[KeyCode::Slash] = KeyState();
        states[KeyCode::Alpha0] = KeyState();
        states[KeyCode::Alpha1] = KeyState();
        states[KeyCode::Alpha2] = KeyState();
        states[KeyCode::Alpha3] = KeyState();
        states[KeyCode::Alpha4] = KeyState();
        states[KeyCode::Alpha5] = KeyState();
        states[KeyCode::Alpha6] = KeyState();
        states[KeyCode::Alpha7] = KeyState();
        states[KeyCode::Alpha8] = KeyState();
        states[KeyCode::Alpha9] = KeyState();
        states[KeyCode::SemiColon] = KeyState();
        states[KeyCode::Equal] = KeyState();
        states[KeyCode::A] = KeyState();
        states[KeyCode::B] = KeyState();
        states[KeyCode::C] = KeyState();
        states[KeyCode::D] = KeyState();
        states[KeyCode::E] = KeyState();
        states[KeyCode::F] = KeyState();
        states[KeyCode::G] = KeyState();
        states[KeyCode::H] = KeyState();
        states[KeyCode::I] = KeyState();
        states[KeyCode::J] = KeyState();
        states[KeyCode::K] = KeyState();
        states[KeyCode::L] = KeyState();
        states[KeyCode::M] = KeyState();
        states[KeyCode::N] = KeyState();
        states[KeyCode::O] = KeyState();
        states[KeyCode::P] = KeyState();
        states[KeyCode::Q] = KeyState();
        states[KeyCode::R] = KeyState();
        states[KeyCode::S] = KeyState();
        states[KeyCode::T] = KeyState();
        states[KeyCode::U] = KeyState();
        states[KeyCode::V] = KeyState();
        states[KeyCode::W] = KeyState();
        states[KeyCode::X] = KeyState();
        states[KeyCode::Y] = KeyState();
        states[KeyCode::Z] = KeyState();
        states[KeyCode::LeftBracket] = KeyState();
        states[KeyCode::BackSlash] = KeyState();
        states[KeyCode::RightBracket] = KeyState();
        states[KeyCode::GraveAccent] = KeyState();
        states[KeyCode::World1] = KeyState();
        states[KeyCode::World2] = KeyState();
        states[KeyCode::Escape] = KeyState();
        states[KeyCode::Enter] = KeyState();
        states[KeyCode::Tab] = KeyState();
        states[KeyCode::Backspace] = KeyState();
        states[KeyCode::Insert] = KeyState();
        states[KeyCode::Delete] = KeyState();
        states[KeyCode::Right] = KeyState();
        states[KeyCode::Left] = KeyState();
        states[KeyCode::Down] = KeyState();
        states[KeyCode::Up] = KeyState();
        states[KeyCode::PageUp] = KeyState();
        states[KeyCode::PageDown] = KeyState();
        states[KeyCode::Home] = KeyState();
        states[KeyCode::End] = KeyState();
        states[KeyCode::CapsLock] = KeyState();
        states[KeyCode::ScrollLock] = KeyState();
        states[KeyCode::NumLock] = KeyState();
        states[KeyCode::PrintScreen] = KeyState();
        states[KeyCode::Pause] = KeyState();
        states[KeyCode::F1] = KeyState();
        states[KeyCode::F2] = KeyState();
        states[KeyCode::F3] = KeyState();
        states[KeyCode::F4] = KeyState();
        states[KeyCode::F5] = KeyState();
        states[KeyCode::F6] = KeyState();
        states[KeyCode::F7] = KeyState();
        states[KeyCode::F8] = KeyState();
        states[KeyCode::F9] = KeyState();
        states[KeyCode::F10] = KeyState();
        states[KeyCode::F11] = KeyState();
        states[KeyCode::F12] = KeyState();
        states[KeyCode::F13] = KeyState();
        states[KeyCode::F14] = KeyState();
        states[KeyCode::F15] = KeyState();
        states[KeyCode::F16] = KeyState();
        states[KeyCode::F17] = KeyState();
        states[KeyCode::F18] = KeyState();
        states[KeyCode::F19] = KeyState();
        states[KeyCode::F20] = KeyState();
        states[KeyCode::F21] = KeyState();
        states[KeyCode::F22] = KeyState();
        states[KeyCode::F23] = KeyState();
        states[KeyCode::F24] = KeyState();
        states[KeyCode::F25] = KeyState();
        states[KeyCode::Keypad0] = KeyState();
        states[KeyCode::Keypad1] = KeyState();
        states[KeyCode::Keypad2] = KeyState();
        states[KeyCode::Keypad3] = KeyState();
        states[KeyCode::Keypad4] = KeyState();
        states[KeyCode::Keypad5] = KeyState();
        states[KeyCode::Keypad6] = KeyState();
        states[KeyCode::Keypad7] = KeyState();
        states[KeyCode::Keypad8] = KeyState();
        states[KeyCode::Keypad9] = KeyState();
        states[KeyCode::Decimal] = KeyState();
        states[KeyCode::Divide] = KeyState();
        states[KeyCode::Multiply] = KeyState();
        states[KeyCode::Subtract] = KeyState();
        states[KeyCode::Add] = KeyState();
        states[KeyCode::KeypadEnter] = KeyState();
        states[KeyCode::KeypadEqual] = KeyState();
        states[KeyCode::LeftShift] = KeyState();
        states[KeyCode::LeftControl] = KeyState();
        states[KeyCode::LeftAlt] = KeyState();
        states[KeyCode::LeftSuper] = KeyState();
        states[KeyCode::RightShift] = KeyState();
        states[KeyCode::RightControl] = KeyState();
        states[KeyCode::RightAlt] = KeyState();
        states[KeyCode::RightSuper] = KeyState();
        states[KeyCode::Menu] = KeyState();
    }
    
    void Keyboard::newFrame() {
        for(auto &k : states) {
            KeyCode key = k.first;
            int state = k.second.state;

            if(state > 0) {
                if(states[key].down == 0) {
                    states[key].down = 1;
                    states[key].pressed = 0;
                    states[key].lastRepeatTime = glfwGetTime();
                    states[key].repeat = false;
                    if(keyDown)
                        keyDown(key);
                } else {
                    states[key].down = 1;
                    states[key].pressed = 1;
                    
                    double currentTime = glfwGetTime();
                    double elapsed = currentTime - states[key].lastRepeatTime;

                    if (!states[key].repeat) {
                        if (elapsed >= repeatDelay) {
                            if(keyRepeat)
                                keyRepeat(key);
                            states[key].repeat = true;
                            states[key].lastRepeatTime = currentTime;
                        }
                    } else {
                        if (elapsed >= repeatInterval) {
                            if(keyRepeat)
                                keyRepeat(key);
                            states[key].lastRepeatTime = currentTime;
                        }
                    }
                    
                    if(keyPress)
                        keyPress(key);
                }

                states[key].up = 0;
            } else {
                if(states[key].down == 1 || states[key].pressed == 1) {
                    states[key].down = 0;
                    states[key].pressed = 0;
                    states[key].up = 1;
                    if(keyUp)
                        keyUp(key);
                } else {
                    states[key].down = 0;
                    states[key].pressed = 0;
                    states[key].up = 0;
                }
            }
        }
    }

    void Keyboard::setState(KeyCode keycode, int32_t state) {
        states[keycode].state = state;
    }

    void Keyboard::addInputCharacter(uint32_t codepoint) {
        if(codepoint > 0) {
            if(charPress)
                charPress(codepoint);
        }
    }

    bool Keyboard::getKey(KeyCode keycode) {
        KeyState state = states[keycode];
        return state.down == 1 && state.pressed == 1;
    }

    bool Keyboard::getKeyDown(KeyCode keycode) {
        KeyState state = states[keycode];
        return state.down == 1 && state.pressed == 0;
    }

    bool Keyboard::getKeyUp(KeyCode keycode) {
        KeyState state = states[keycode];
        return state.up > 0;
    }

    bool Keyboard::isAnyKeyPressed() {
        for(auto &k : states) {
            if(k.second.pressed > 0)
                return true;
        }

        return false;
    }
}