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
};