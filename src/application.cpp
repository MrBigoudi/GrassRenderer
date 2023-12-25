#include "application.hpp"

#include <cstdlib>
#include <iostream>

void Application::initGLAD(){
    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to init GLAD!\n");
        exit(EXIT_FAILURE);
    }
}

void Application::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _Window = glfwCreateWindow(800, 600, "grass renderer", NULL, NULL);
    if(!_Window){
        fprintf(stderr, "Failed to init GLFW window!\n");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_Window);
}

void Application::update(float dt){
}

void Application::render(){
}

void Application::handleInput(){
    if(glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(_Window, true);
    }
}

void Application::init(){
    initGLFW();
    initGLAD();
}

void Application::run(){
    while(!glfwWindowShouldClose(_Window)){
        glfwSwapBuffers(_Window);
        handleInput();
        update(0.0f);
        render();
        glfwPollEvents();
    }
}

void Application::quit(){
    glfwTerminate();
}