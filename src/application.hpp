#pragma once

#include "computeShader.hpp"
#include "shaders.hpp"

#include <glad/gl.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

class Application{
    private:
        GLFWwindow* _Window = nullptr;
        ShadersPointer _Shaders = nullptr;
        ComputeShader* _ComputeShader = nullptr;

    private:
        void update(float dt);
        void render();
        void initGLFW();
        void initGLAD();
        void initShaders();
        void handleInput();

    public:
        Application(){}

        void init();
        void run();
        void quit();
};