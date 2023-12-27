#include "application.hpp"
#include "errorHandler.hpp"
#include "grass.hpp"
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
    // _Shaders = ShadersPointer( new Shaders("shader/vert.glsl", "shader/frag.glsl"));
    _Shaders = ShadersPointer( new Shaders("shader/vert.glsl", "shader/frag.glsl", "shader/geom.glsl"));
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
}

void Application::update(){
    updateDt();
}

void Application::render(){
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _Grass->render(_Shaders.get());
}

void Application::handleInput(){
    if(glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(_Window, true);
    }

    if (glfwGetKey(_Window, GLFW_KEY_W) == GLFW_PRESS)
        _Camera->ProcessKeyboard(FORWARD, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_S) == GLFW_PRESS)
        _Camera->ProcessKeyboard(BACKWARD, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_A) == GLFW_PRESS)
        _Camera->ProcessKeyboard(LEFT, _DeltaTime);
    if (glfwGetKey(_Window, GLFW_KEY_D) == GLFW_PRESS)
        _Camera->ProcessKeyboard(RIGHT, _DeltaTime);
}

void Application::init(){
    initGLFW();
    initGLAD();
    initShaders();
    _Camera = new Camera();
    _Grass = new Grass();
}

void Application::run(){

    while(!glfwWindowShouldClose(_Window)){
        handleInput();
        update();

        // test camera
        _Shaders->use();
        glm::mat4 projection = 
            glm::perspective(
                glm::radians(_Camera->Zoom), 
                (float)_Width / (float)_Height, 
                0.1f, 100.0f
        );
        _Shaders->setMat4f("projection", projection);
        glm::mat4 view = _Camera->GetViewMatrix();
        _Shaders->setMat4f("view", view);
        // end of test

        // test compute shader
        _Grass->dispatchComputeShader();
        //

        render();

        glfwPollEvents();
        glfwSwapBuffers(_Window);
    }
}

void Application::quit(){
    glfwTerminate();
}