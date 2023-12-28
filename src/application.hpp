#pragma once

#include "axis.hpp"
#include "camera.hpp"
#include "shaders.hpp"
#include "grass.hpp"

#include <glad/gl.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

class Application{
    private:
        GLFWwindow* _Window = nullptr;
        GLuint _Width = 800;
        GLuint _Height = 600;
        ShadersPointer _Shaders = nullptr;

        Camera* _Camera = nullptr;
        Grass* _Grass = nullptr;
        Axis* _Axis = nullptr;

        float _CurrentFrameTime = 0.0f;
        float _LastFrameTime = 0.0f;
        float _DeltaTime = 0.0f;

        bool _FirstMouse = true;
        double _LastMouseX;
        double _LastMouseY;

    private:
        void update();
        void render();
        void initGLFW();
        void initGLAD();
        void initShaders();
        void handleInput();
        void handleCameraInput();

        void updateDt(){
            _CurrentFrameTime = static_cast<float>(glfwGetTime());
            _DeltaTime = _CurrentFrameTime - _LastFrameTime;
            _LastFrameTime = _CurrentFrameTime;
        }

    public:
        Application(){}

        void init();
        void run();
        void quit();

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

            if (app->_FirstMouse) {
                app->_LastMouseX = xpos;
                app->_LastMouseY = ypos;
                app->_FirstMouse = false;
            }

            double xoffset = xpos - app->_LastMouseX;
            double yoffset = app->_LastMouseY - ypos; // reversed since y-coordinates range from bottom to top

            app->_LastMouseX = xpos;
            app->_LastMouseY = ypos;

            app->_Camera->ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
        }
};