#include "application.hpp"
#include "errorHandler.hpp"
#include "shaders.hpp"

#include <cstdlib>
#include <iostream>

void Application::initGLAD(){
    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to init GLAD!\n");
        ErrorHandler::handle(ErrorCodes::GLAD_ERROR);
        exit(EXIT_FAILURE);
    }
}

void Application::initShaders(){
    _Shaders = ShadersPointer( new Shaders("shader/basic.vert", "shader/basic.frag"));
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
        ErrorHandler::handle(ErrorCodes::GLFW_ERROR);
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_Window);
}

void Application::update(float dt){
}

void Application::render(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
        handleInput();
        update(0.0f);
        render();

        glfwPollEvents();
        glfwSwapBuffers(_Window);
    }
}

void Application::quit(){
    glfwTerminate();
}