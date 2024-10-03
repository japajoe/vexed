#ifndef VEXED_KEYBOARD_H
#define VEXED_KEYBOARD_H

#include <cstdint>
#include <unordered_map>
#include <functional>

namespace vexed {
    /* Printable keys */
    //==============================================================================
    #define GLFW__KEY_UNKNOWN            -1
    #define GLFW__KEY_SPACE              32
    #define GLFW__KEY_APOSTROPHE         39  /* ' */
    #define GLFW__KEY_COMMA              44  /* , */
    #define GLFW__KEY_MINUS              45  /* - */
    #define GLFW__KEY_PERIOD             46  /* . */
    #define GLFW__KEY_SLASH              47  /* / */
    #define GLFW__KEY_0                  48
    #define GLFW__KEY_1                  49
    #define GLFW__KEY_2                  50
    #define GLFW__KEY_3                  51
    #define GLFW__KEY_4                  52
    #define GLFW__KEY_5                  53
    #define GLFW__KEY_6                  54
    #define GLFW__KEY_7                  55
    #define GLFW__KEY_8                  56
    #define GLFW__KEY_9                  57
    #define GLFW__KEY_SEMICOLON          59  /* ; */
    #define GLFW__KEY_EQUAL              61  /* = */
    #define GLFW__KEY_A                  65
    #define GLFW__KEY_B                  66
    #define GLFW__KEY_C                  67
    #define GLFW__KEY_D                  68
    #define GLFW__KEY_E                  69
    #define GLFW__KEY_F                  70
    #define GLFW__KEY_G                  71
    #define GLFW__KEY_H                  72
    #define GLFW__KEY_I                  73
    #define GLFW__KEY_J                  74
    #define GLFW__KEY_K                  75
    #define GLFW__KEY_L                  76
    #define GLFW__KEY_M                  77
    #define GLFW__KEY_N                  78
    #define GLFW__KEY_O                  79
    #define GLFW__KEY_P                  80
    #define GLFW__KEY_Q                  81
    #define GLFW__KEY_R                  82
    #define GLFW__KEY_S                  83
    #define GLFW__KEY_T                  84
    #define GLFW__KEY_U                  85
    #define GLFW__KEY_V                  86
    #define GLFW__KEY_W                  87
    #define GLFW__KEY_X                  88
    #define GLFW__KEY_Y                  89
    #define GLFW__KEY_Z                  90
    #define GLFW__KEY_LEFT_BRACKET       91  /* [ */
    #define GLFW__KEY_BACKSLASH          92  /* \ */
    #define GLFW__KEY_RIGHT_BRACKET      93  /* ] */
    #define GLFW__KEY_GRAVE_ACCENT       96  /* ` */
    #define GLFW__KEY_WORLD_1            161 /* non-US #1 */
    #define GLFW__KEY_WORLD_2            162 /* non-US #2 */

    /* Function keys */
    //==============================================================================
    #define GLFW__KEY_ESCAPE             256
    #define GLFW__KEY_ENTER              257
    #define GLFW__KEY_TAB                258
    #define GLFW__KEY_BACKSPACE          259
    #define GLFW__KEY_INSERT             260
    #define GLFW__KEY_DELETE             261
    #define GLFW__KEY_RIGHT              262
    #define GLFW__KEY_LEFT               263
    #define GLFW__KEY_DOWN               264
    #define GLFW__KEY_UP                 265
    #define GLFW__KEY_PAGE_UP            266
    #define GLFW__KEY_PAGE_DOWN          267
    #define GLFW__KEY_HOME               268
    #define GLFW__KEY_END                269
    #define GLFW__KEY_CAPS_LOCK          280
    #define GLFW__KEY_SCROLL_LOCK        281
    #define GLFW__KEY_NUM_LOCK           282
    #define GLFW__KEY_PRINT_SCREEN       283
    #define GLFW__KEY_PAUSE              284
    #define GLFW__KEY_F1                 290
    #define GLFW__KEY_F2                 291
    #define GLFW__KEY_F3                 292
    #define GLFW__KEY_F4                 293
    #define GLFW__KEY_F5                 294
    #define GLFW__KEY_F6                 295
    #define GLFW__KEY_F7                 296
    #define GLFW__KEY_F8                 297
    #define GLFW__KEY_F9                 298
    #define GLFW__KEY_F10                299
    #define GLFW__KEY_F11                300
    #define GLFW__KEY_F12                301
    #define GLFW__KEY_F13                302
    #define GLFW__KEY_F14                303
    #define GLFW__KEY_F15                304
    #define GLFW__KEY_F16                305
    #define GLFW__KEY_F17                306
    #define GLFW__KEY_F18                307
    #define GLFW__KEY_F19                308
    #define GLFW__KEY_F20                309
    #define GLFW__KEY_F21                310
    #define GLFW__KEY_F22                311
    #define GLFW__KEY_F23                312
    #define GLFW__KEY_F24                313
    #define GLFW__KEY_F25                314
    #define GLFW__KEY_KP_0               320
    #define GLFW__KEY_KP_1               321
    #define GLFW__KEY_KP_2               322
    #define GLFW__KEY_KP_3               323
    #define GLFW__KEY_KP_4               324
    #define GLFW__KEY_KP_5               325
    #define GLFW__KEY_KP_6               326
    #define GLFW__KEY_KP_7               327
    #define GLFW__KEY_KP_8               328
    #define GLFW__KEY_KP_9               329
    #define GLFW__KEY_KP_DECIMAL         330
    #define GLFW__KEY_KP_DIVIDE          331
    #define GLFW__KEY_KP_MULTIPLY        332
    #define GLFW__KEY_KP_SUBTRACT        333
    #define GLFW__KEY_KP_ADD             334
    #define GLFW__KEY_KP_ENTER           335
    #define GLFW__KEY_KP_EQUAL           336
    #define GLFW__KEY_LEFT_SHIFT         340
    #define GLFW__KEY_LEFT_CONTROL       341
    #define GLFW__KEY_LEFT_ALT           342
    #define GLFW__KEY_LEFT_SUPER         343
    #define GLFW__KEY_RIGHT_SHIFT        344
    #define GLFW__KEY_RIGHT_CONTROL      345
    #define GLFW__KEY_RIGHT_ALT          346
    #define GLFW__KEY_RIGHT_SUPER        347
    #define GLFW__KEY_MENU               348
    #define GLFW__KEY_LAST               GLFW__KEY_MENU

    enum class KeyCode {
        Unknown = GLFW__KEY_UNKNOWN,
        Space = GLFW__KEY_SPACE,
        Apostrophe = GLFW__KEY_APOSTROPHE,
        Comma = GLFW__KEY_COMMA,
        Minus = GLFW__KEY_MINUS,
        Period = GLFW__KEY_PERIOD,
        Slash = GLFW__KEY_SLASH,
        Alpha0 = GLFW__KEY_0,
        Alpha1 = GLFW__KEY_1,
        Alpha2 = GLFW__KEY_2,
        Alpha3 = GLFW__KEY_3,
        Alpha4 = GLFW__KEY_4,
        Alpha5 = GLFW__KEY_5,
        Alpha6 = GLFW__KEY_6,
        Alpha7 = GLFW__KEY_7,
        Alpha8 = GLFW__KEY_8,
        Alpha9 = GLFW__KEY_9,
        SemiColon = GLFW__KEY_SEMICOLON,
        Equal = GLFW__KEY_EQUAL,
        A = GLFW__KEY_A,
        B = GLFW__KEY_B,
        C = GLFW__KEY_C,
        D = GLFW__KEY_D,
        E = GLFW__KEY_E,
        F = GLFW__KEY_F,
        G = GLFW__KEY_G,
        H = GLFW__KEY_H,
        I = GLFW__KEY_I,
        J = GLFW__KEY_J,
        K = GLFW__KEY_K,
        L = GLFW__KEY_L,
        M = GLFW__KEY_M,
        N = GLFW__KEY_N,
        O = GLFW__KEY_O,
        P = GLFW__KEY_P,
        Q = GLFW__KEY_Q,
        R = GLFW__KEY_R,
        S = GLFW__KEY_S,
        T = GLFW__KEY_T,
        U = GLFW__KEY_U,
        V = GLFW__KEY_V,
        W = GLFW__KEY_W,
        X = GLFW__KEY_X,
        Y = GLFW__KEY_Y,
        Z = GLFW__KEY_Z,
        LeftBracket = GLFW__KEY_LEFT_BRACKET,
        BackSlash = GLFW__KEY_BACKSLASH,
        RightBracket = GLFW__KEY_RIGHT_BRACKET,
        GraveAccent = GLFW__KEY_GRAVE_ACCENT,    
        World1 = GLFW__KEY_UNKNOWN,
        World2 = GLFW__KEY_UNKNOWN,
        Escape = GLFW__KEY_ESCAPE,
        Enter = GLFW__KEY_ENTER,
        Tab = GLFW__KEY_TAB,
        Backspace = GLFW__KEY_BACKSPACE,
        Insert = GLFW__KEY_INSERT,
        Delete = GLFW__KEY_DELETE,
        Right = GLFW__KEY_RIGHT,
        Left = GLFW__KEY_LEFT,
        Down = GLFW__KEY_DOWN,
        Up = GLFW__KEY_UP,
        PageUp = GLFW__KEY_PAGE_UP,
        PageDown = GLFW__KEY_PAGE_DOWN,
        Home = GLFW__KEY_HOME,
        End = GLFW__KEY_END,
        CapsLock = GLFW__KEY_CAPS_LOCK,
        ScrollLock = GLFW__KEY_SCROLL_LOCK,
        NumLock = GLFW__KEY_NUM_LOCK,
        PrintScreen = GLFW__KEY_PRINT_SCREEN,
        Pause = GLFW__KEY_PAUSE,
        F1 = GLFW__KEY_F1,
        F2 = GLFW__KEY_F2,
        F3 = GLFW__KEY_F3,
        F4 = GLFW__KEY_F4,
        F5 = GLFW__KEY_F5,
        F6 = GLFW__KEY_F6,
        F7 = GLFW__KEY_F7,
        F8 = GLFW__KEY_F8,
        F9 = GLFW__KEY_F9,
        F10 = GLFW__KEY_F10,
        F11 = GLFW__KEY_F11,
        F12 = GLFW__KEY_F12,
        F13 = GLFW__KEY_F13,
        F14 = GLFW__KEY_F14,
        F15 = GLFW__KEY_F15,
        F16 = GLFW__KEY_F16,
        F17 = GLFW__KEY_F17,
        F18 = GLFW__KEY_F18,
        F19 = GLFW__KEY_F19,
        F20 = GLFW__KEY_F20,
        F21 = GLFW__KEY_F21,
        F22 = GLFW__KEY_F22,
        F23 = GLFW__KEY_F23,
        F24 = GLFW__KEY_F24,
        F25 = GLFW__KEY_UNKNOWN,
        Keypad0 = GLFW__KEY_KP_0,
        Keypad1 = GLFW__KEY_KP_1,
        Keypad2 = GLFW__KEY_KP_2,
        Keypad3 = GLFW__KEY_KP_3,
        Keypad4 = GLFW__KEY_KP_4,
        Keypad5 = GLFW__KEY_KP_5,
        Keypad6 = GLFW__KEY_KP_6,
        Keypad7 = GLFW__KEY_KP_7,
        Keypad8 = GLFW__KEY_KP_8,
        Keypad9 = GLFW__KEY_KP_9,
        Decimal = GLFW__KEY_KP_DECIMAL,
        Divide = GLFW__KEY_KP_DIVIDE,
        Multiply = GLFW__KEY_KP_MULTIPLY,
        Subtract = GLFW__KEY_KP_SUBTRACT,
        Add = GLFW__KEY_KP_ADD,
        KeypadEnter = GLFW__KEY_KP_ENTER,
        KeypadEqual = GLFW__KEY_KP_EQUAL,
        LeftShift = GLFW__KEY_LEFT_SHIFT,
        LeftControl = GLFW__KEY_LEFT_CONTROL,
        LeftAlt = GLFW__KEY_LEFT_ALT,
        LeftSuper = GLFW__KEY_LEFT_SUPER,
        RightShift = GLFW__KEY_RIGHT_SHIFT,
        RightControl = GLFW__KEY_RIGHT_CONTROL,
        RightAlt = GLFW__KEY_RIGHT_ALT,
        RightSuper = GLFW__KEY_RIGHT_SUPER,
        Menu = GLFW__KEY_MENU
    };

    struct KeyState {
        int down;
        int up;
        int pressed;
        int state;
        double lastRepeatTime;
        bool repeat;

        KeyState() {
            down = 0;
            up = 0;
            pressed = 0;
            state = 0;
            lastRepeatTime = 0.0;
            repeat = false;
        }
    };

    using CharPressEvent = std::function<void(uint32_t codepoint)>;
    using KeyDownEvent = std::function<void(KeyCode keycode)>;
    using KeyUpEvent = std::function<void(KeyCode keycode)>;
    using KeyPressEvent = std::function<void(KeyCode keycode)>;
    using KeyRepeatEvent = std::function<void(KeyCode keycode)>;

    class Keyboard {
    public:
        CharPressEvent charPress;
        KeyDownEvent keyDown;
        KeyUpEvent keyUp;
        KeyPressEvent keyPress;
        KeyRepeatEvent keyRepeat;
        Keyboard();
        void initialize();
        void newFrame();
        void setState(KeyCode keycode, int32_t state);
        void addInputCharacter(uint32_t codepoint);
        bool getKey(KeyCode keycode);
        bool getKeyDown(KeyCode keycode);
        bool getKeyUp(KeyCode keycode);
        bool isAnyKeyPressed();
    private:
        std::unordered_map<KeyCode,KeyState> states;
        double repeatDelay;
        double repeatInterval;
    };
}

#endif