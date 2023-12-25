#pragma once

#include <glad/gl.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

class Application{
    private:
        GLFWwindow* _Window = nullptr;

    private:
        void update(float dt);
        void render();
        void initGLFW();
        void initGLAD();
        void handleInput();

    public:
        Application(){}

        void init();
        void run();
        void quit();
};