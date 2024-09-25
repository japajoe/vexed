#ifndef VEXED_APPLICATION_H
#define VEXED_APPLICATION_H

#include "graphics.h"
#include "keyboard.h"
#include "mouse.h"
#include <string>
#include <cstdint>
#include <functional>
#include <chrono>

struct GLFWwindow;
struct GLFWmonitor;

namespace vexed {
    class Application;

    using UpdateCallback = std::function<void(Application *application)>;
    using LoadCallback = std::function<void(Application *application)>;
    using CloseCallback = std::function<void(Application *application)>;

    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimerInstance;

    struct Timer {
        TimerInstance tp1;
        TimerInstance tp2;
        float deltaTime;
        
        Timer() {
            tp1 = std::chrono::system_clock::now();
            tp1 = std::chrono::system_clock::now();
            deltaTime = 0;
        }
        
        void update() {
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsed = tp2 - tp1;
            tp1 = tp2;
            deltaTime = elapsed.count();
        }
    };

    enum WindowFlags_ {
        WindowFlags_None = 0,
        WindowFlags_VSync = 1 << 0,
        WindowFlags_Maximize = 1 << 1,
        WindowFlags_Fullscreen = 1 << 2
    };

    typedef int WindowFlags;

    struct Configuration {
        std::string title;
        uint32_t width;
        uint32_t height;
        WindowFlags flags;
        uint8_t *iconData; //Needs to be data of an encoded PNG/JPEG
        size_t iconDataSize;
    };

    class Application {
    public:
        UpdateCallback update;
        LoadCallback load;
        CloseCallback close;
        Application();
        Application(const Configuration &config);
        void run();
        inline Graphics *getGraphics() { return &graphics; }
        inline Keyboard *getKeyboard() { return &keyboard; }
        inline Mouse *getMouse() { return &mouse; }
        inline float getDeltaTime() const { return timer.deltaTime; }
        inline float getFPS() const { return averageFPS; }
    private:
        Configuration config;
        GLFWwindow *window;
        GLFWmonitor *monitor;
        Graphics graphics;
        Keyboard keyboard;
        Mouse mouse;
        Timer timer;
        float averageFPS;
        static void onFramebufferResize(GLFWwindow* window, int width, int height);
        static void onKeyPress(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
        static void onCharPress(GLFWwindow* window, uint32_t codepoint);
        static void onMouseButtonPress(GLFWwindow *window, int32_t button, int32_t action, int32_t mods);
        static void onMouseMove(GLFWwindow *window, double x, double y);
        static void onMouseScroll(GLFWwindow *window, double xoffset, double yoffset);
    };
}

#endif