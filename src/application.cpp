#include "application.hpp"
#include "camera.hpp"
#include "errorHandler.hpp"
#include "grass.hpp"
#include "shaders.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/fwd.hpp>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void Application::initGLAD(){
    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to init GLAD!\n");
        ErrorHandler::handle(ErrorCodes::GLAD_ERROR);
        exit(EXIT_FAILURE);
    }
}

void Application::initShaders(){
    _Shaders = ShadersPointer( new Shaders("shader/grassVert.glsl", "shader/grassFrag.glsl", "shader/grassGeom.glsl"));
}


void Application::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    _Window = glfwCreateWindow(_Width, _Height, "grass renderer", NULL, NULL);
    if(!_Window){
        fprintf(stderr, "Failed to init GLFW window!\n");
        ErrorHandler::handle(ErrorCodes::GLFW_ERROR);
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_Window);

    glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_Window, mouseCallback);
    glfwSetWindowUserPointer(_Window, this);
}

void Application::update(){
    updateDt();
    _Grass->update(_DeltaTime, _Camera->getPosition());
}

void Application::render(const glm::mat4& view, const glm::mat4& proj){
    glClearColor(0.383f, 0.632f, 0.800f, 1.0f);
    // glClearColor(0.f, 0.f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _Grass->render(_Shaders.get(), _Camera, view, proj);
    _Axis->render(view, proj);
    _Sun->render(view, proj);
}

void Application::handleCameraInput(){
    // move camera
    if (glfwGetKey(_Window, GLFW_KEY_W) == GLFW_PRESS)
        _Camera->processKeyboard(FORWARD, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_S) == GLFW_PRESS)
        _Camera->processKeyboard(BACKWARD, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_A) == GLFW_PRESS)
        _Camera->processKeyboard(LEFT, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_D) == GLFW_PRESS)
        _Camera->processKeyboard(RIGHT, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_UP) == GLFW_PRESS)
        _Camera->processKeyboard(UP, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_DOWN) == GLFW_PRESS)
        _Camera->processKeyboard(DOWN, _DeltaTime);

    // accelerate camera
    if (glfwGetKey(_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        _Camera->_Accelerate = true;
    }
    if (glfwGetKey(_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
        _Camera->_Accelerate = false;
    }

}

void Application::handleInput(){
    if(glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(_Window, true);
    }
    handleWireFrameInput();
    handleCameraInput();
}

void Application::init(){
    initGLFW();
    initGLAD();
    initShaders();
    _Axis = new Axis();
    _Grass = new Grass();
    _Camera = new Camera(_Grass->getCenter(), (float)_Width / (float)_Height);
    initLights();

    glEnable(GL_DEPTH_TEST);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to enable z-buffer: %d\n", error);
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
    glDisable(GL_CULL_FACE);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to disable backface culling : %d\n", error);
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

void Application::run(){

    while(!glfwWindowShouldClose(_Window)){
        handleInput();
        update();

        _Shaders->use();
        glm::mat4 projection = _Camera->getPerspective();
        glm::mat4 view = _Camera->getView();
        render(view, projection);

        glfwPollEvents();
        glfwSwapBuffers(_Window);

    }
}

void Application::quit(){
    glfwTerminate();
}