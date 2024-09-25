#include "application.h"
#include "image.h"
#include "../glad/glad.h"
#include "../glfw/glfw3.h"
#include <iostream>

namespace vexed {
    Application::Application() 
        : window(nullptr), monitor(nullptr) {
        config.title = "Vexed";
        config.width = 512;
        config.height = 512;
        config.flags = WindowFlags_VSync;
        config.iconData = nullptr;
        config.iconDataSize = 0;
    }

    Application::Application(const Configuration &config) 
        : config(config), window(nullptr), monitor(nullptr) {
    }
    
    void Application::run() {
        if(window) {
            std::cerr << "Window is already created" << std::endl;
            return;
        }

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_MAXIMIZED, config.flags & WindowFlags_Maximize);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
        
        monitor = glfwGetPrimaryMonitor();

        if(config.flags & WindowFlags_Fullscreen) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            window = glfwCreateWindow(mode->width, mode->height, config.title.c_str(), monitor, nullptr);
        } else {
            window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
        }
        
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            window = nullptr;
        }

        glEnable(GL_MULTISAMPLE);

        if(config.iconData != nullptr && config.iconDataSize > 0) {
            Image image(config.iconData, config.iconDataSize);

            if(image.isLoaded()) {
                GLFWimage windowIcon;
                windowIcon.width = image.getWidth();
                windowIcon.height = image.getHeight();
                windowIcon.pixels = image.getData();
                glfwSetWindowIcon(window, 1, &windowIcon);
            }
        }

        glfwSwapInterval(config.flags & WindowFlags_VSync);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, onFramebufferResize);
        glfwSetKeyCallback(window, onKeyPress);
        glfwSetCharCallback(window, onCharPress);
        glfwSetMouseButtonCallback(window, onMouseButtonPress);
        glfwSetCursorPosCallback(window, onMouseMove);
        glfwSetScrollCallback(window, onMouseScroll);

        graphics.initialize();
        graphics.setClearColor(Color(0, 0, 0, 1));
        graphics.setViewport(0, 0, config.width, config.height);

        keyboard.initialize();
        mouse.initialize();

        if(load)
            load(this);

        float elapsedTime = 0.0f;
        int fps = 0;

        while (!glfwWindowShouldClose(window)) {
            timer.update();

            elapsedTime += timer.deltaTime;
            fps++;

            if (elapsedTime >= 0.5f) {
                averageFPS = fps / elapsedTime;
                fps = 0;
                elapsedTime = 0;
            }

            keyboard.newFrame();
            mouse.newFrame();

            if(update)
                update(this);

            graphics.newFrame(timer.deltaTime);

            mouse.endFrame();
            
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if(close)
            close(this);

        graphics.deinitialize();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Application::onFramebufferResize(GLFWwindow* window, int width, int height) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->graphics.setViewport(0, 0, width, height);
        }
    }

    void Application::onKeyPress(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->keyboard.setState(static_cast<KeyCode>(key), action > 0 ? 1 : 0);
        }
    }

    void Application::onCharPress(GLFWwindow* window, uint32_t codepoint) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->keyboard.addInputCharacter(codepoint);
        }
    }

    void Application::onMouseButtonPress(GLFWwindow *window, int32_t button, int32_t action, int32_t mods) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->mouse.setState(static_cast<ButtonCode>(button), action > 0 ? 1 : 0);
        }
    }

    void Application::onMouseMove(GLFWwindow *window, double x, double y) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->mouse.setPosition(x, y);
        }
    }

    void Application::onMouseScroll(GLFWwindow *window, double xoffset, double yoffset) {
        void *userData = glfwGetWindowUserPointer(window);
        if(userData) {
            Application *application = reinterpret_cast<Application*>(userData);
            application->mouse.setScrollDirection(xoffset, yoffset);
        }
    }
}