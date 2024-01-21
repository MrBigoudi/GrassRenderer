#include "application.hpp"
#include "camera.hpp"
#include "errorHandler.hpp"
#include "grass.hpp"
#include "gui.hpp"
#include "shaders.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/fwd.hpp>
#include <iostream>

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
    // center window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = mode->width;
    int screenHeight = mode->height;
    int windowWidth = _Width;
    int windowHeight = _Height;
    int windowXPos = (screenWidth - windowWidth) / 2;
    int windowYPos = (screenHeight - windowHeight) / 2;
    glfwSetWindowPos(_Window, windowXPos, windowYPos);

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
    // Check if the 'M' key is pressed
    if (glfwGetKey(_Window, GLFW_KEY_J) == GLFW_PRESS) {
        if(!_isPressedJ){
            _isPressedJ = true;
            if(_MouseMode == MOUSE_MODE_CAMERA){
                glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                _MouseMode = MOUSE_MODE_WINDOW;
            } else {
                glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                _MouseMode = MOUSE_MODE_CAMERA;
            }
        }
    }
    if (glfwGetKey(_Window, GLFW_KEY_G) == GLFW_PRESS) {
        if(!_isPressedG){
            _isPressedG = true;
            _ImGuiShowAnalytics = !_ImGuiShowAnalytics;
        }
    }
    if (glfwGetKey(_Window, GLFW_KEY_H) == GLFW_PRESS) {
        if(!_isPressedH){
            _isPressedH = true;
            _ImGuiShowHelp = !_ImGuiShowHelp;
        }
    }

    if (glfwGetKey(_Window, GLFW_KEY_J) == GLFW_RELEASE) {
        if(_isPressedJ){
            _isPressedJ = false;
        }        
    }
    if (glfwGetKey(_Window, GLFW_KEY_G) == GLFW_RELEASE) {
        if(_isPressedG){
            _isPressedG = false;
        }        
    }
    if (glfwGetKey(_Window, GLFW_KEY_H) == GLFW_RELEASE) {
        if(_isPressedH){
            _isPressedH = false;
        }        
    }

    if (glfwGetKey(_Window, GLFW_KEY_V) == GLFW_PRESS) {
        if(!_isPressedV){
            _SaveFrame = !_SaveFrame;
            if(_SaveFrame) _SaveVideoCount++;
        }
    }
    if (glfwGetKey(_Window, GLFW_KEY_V) == GLFW_RELEASE) {
        if(_isPressedV){
            _isPressedV = false;
        }        
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

    initGUI();
}

void Application::run(){

    while(!glfwWindowShouldClose(_Window)){
        handleInput();
        update();

        _Shaders->use();
        glm::mat4 projection = _Camera->getPerspective();
        glm::mat4 view = _Camera->getView();
        render(view, projection);

        renderGUI();
        glfwPollEvents();
        glfwSwapBuffers(_Window);

        // saveFrame();
    }
}

void Application::quit(){
    cleanIMGUI();
    glfwDestroyWindow(_Window);
    glfwTerminate();
}