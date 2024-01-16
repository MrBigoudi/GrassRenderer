#pragma once

#include "axis.hpp"
#include "camera.hpp"
#include "errorHandler.hpp"
#include "shaders.hpp"
#include "grass.hpp"
#include "light.hpp"
#include "sun.hpp"

#include <chrono>
#include <glad/gl.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <thread>

using PointLights = std::vector<LightPointer>;

class Application{
    private:
        GLFWwindow* _Window = nullptr;
        GLuint _Width = 800;
        GLuint _Height = 600;
        ShadersPointer _Shaders = nullptr;

        Camera* _Camera = nullptr;
        Grass* _Grass = nullptr;
        Axis* _Axis = nullptr;

        float _CurrentFrameTime = 0.f;
        float _LastFrameTime = 0.f;
        float _DeltaTime = 0.f;
        float _TargetTime = 1.f / 60.f;

        bool _FirstMouse = true;
        double _LastMouseX;
        double _LastMouseY;

        PointLights _PointLights = {};
        GLuint _NbPointLights = 0;
        Sun* _Sun = nullptr;

    private:
        void update();
        void render(const glm::mat4& view, const glm::mat4& proj);
        void initGLFW();
        void initGLAD();
        void initShaders();
        void handleInput();
        void handleCameraInput();

        void syncDt(){
            float sleepTime = _TargetTime - _DeltaTime;
            if(sleepTime > 0.f){
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }

        void updateDt(){
            _CurrentFrameTime = static_cast<float>(glfwGetTime());
            _DeltaTime = _CurrentFrameTime - _LastFrameTime;
            _LastFrameTime = _CurrentFrameTime;
            fprintf(stdout, "FPS: %f\n", 1.f/_DeltaTime);
        }

        void setWireframeMode() const {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        void unsetWireframeMode() const {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        void handleWireFrameInput(){
            if(glfwGetKey(_Window, GLFW_KEY_F) == GLFW_PRESS){
                setWireframeMode();
            }
        }

        void addPointLight(const LightPointer& light){
            if(light->getType() != LightType::PointLight){
                fprintf(stderr, "Must assign a point light to the scene's point lights!");
                ErrorHandler::handle(ErrorCodes::WRONG_TYPE);
                return;
            }
            _PointLights.push_back(light);
            _NbPointLights++;
        }

        void sendPointLight(){
            _Shaders->setInt("nbPointLights", _NbPointLights);
            for(GLuint i=0; i<_NbPointLights; i++){
                std::string name = "pointLights[" + std::to_string(i) + "]";
                _PointLights[i]->sendGPU(_Shaders, name);
            }
        }

        void initLights(){
            glm::vec3 lightPos = glm::vec3(0.f, 10.f, 0.f);
            glm::vec4 lightCol = glm::vec4(1.f, 1.f, 1.f, 1.f);
            addPointLight(
                LightPointer(
                    new Light(LightType::PointLight, lightPos, lightCol)
                )
            );
            // sendPointLight();
            _Sun = new Sun(_PointLights[0]);
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