#pragma once

#include "axis.hpp"
#include "camera.hpp"
#include "errorHandler.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shaders.hpp"
#include "grass.hpp"
#include "light.hpp"
#include "sun.hpp"
#include "gui.hpp"

#include <chrono>
#include <cmath>
#include <glad/gl.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <thread>

using PointLights = std::vector<LightPointer>;

enum MouseMode{
    MOUSE_MODE_CAMERA,
    MOUSE_MODE_WINDOW,
};

class Application{
    private:
        GLFWwindow* _Window = nullptr;
        GLuint _Width = 1280;
        GLuint _Height = 720;
        ShadersPointer _Shaders = nullptr;

        Camera* _Camera = nullptr;
        Grass* _Grass = nullptr;
        Axis* _Axis = nullptr;

        float _CurrentFrameTime = 0.f;
        float _LastFrameTime = 0.f;
        float _DeltaTime = 0.f;
        float _TargetTime = 1.f / 60.f;
        GLuint _MinFPS = 0;
        GLuint _MaxFPS = 0;
        GLuint _AvgFPS = 0;
        float _SampleDuration = 1.f;
        float _Duration = 0.f;
        float _MinDuration = INFINITY;
        float _MaxDuration = 0.f;
        GLuint _NbFrames = 1;

        bool _FirstMouse = true;
        double _LastMouseX;
        double _LastMouseY;

        PointLights _PointLights = {};
        GLuint _NbPointLights = 0;
        Sun* _Sun = nullptr;

        ImGuiIO* _ImGuiIo = nullptr;
        bool _ImGuiShowAnalytics = true;
        bool _ImGuiShowHelp = true;

        bool _isPressedG = false;
        bool _isPressedH = false;
        bool _isPressedJ = false;
        MouseMode _MouseMode = MOUSE_MODE_CAMERA;

    private:
        void update();
        void render(const glm::mat4& view, const glm::mat4& proj);
        void initGLFW();
        void initGLAD();
        void initShaders();
        void handleInput();
        void handleCameraInput();

        void updateDt(){
            _CurrentFrameTime = static_cast<float>(glfwGetTime());
            _DeltaTime = _CurrentFrameTime - _LastFrameTime;
            _LastFrameTime = _CurrentFrameTime;
            // update fps for analytics
            _NbFrames++;
            _Duration += _DeltaTime;
            if(_DeltaTime > _MaxDuration){
                _MaxDuration = _DeltaTime;
            }
            if(_DeltaTime < _MinDuration){
                _MinDuration = _DeltaTime;
            }
            // reset
            if(_Duration >= _SampleDuration){
                _MinFPS = 1.f / _MaxDuration;
                _MaxFPS = 1.f / _MinDuration;
                _AvgFPS = _NbFrames / _Duration;
                _NbFrames = 1;
                _Duration = 0.f;
                _MinDuration = INFINITY;
                _MaxDuration = 0.f;
            }
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

        void initGUI(){
            _ImGuiIo = &initIMGUI(_Window);
            _ImGuiIo->FontGlobalScale = 1.5f;
        }

        void renderGUI(){
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // analytics
            if(_ImGuiShowAnalytics){
                ImVec2 size{170, 210};
                ImVec2 pos{_Width - size.x, 0};
                ImGui::SetNextWindowPos(pos);
                ImGui::Begin("Analytics", &_ImGuiShowAnalytics);
                ImGui::SetWindowSize(size);
                ImGui::Text("FPS:\n  Avg: %d\n  Min: %d\n  Max: %d", _AvgFPS, _MinFPS, _MaxFPS);
                ImGui::Text("MS:\n  Avg: %.1f\n  Min: %.1f\n  Max: %.1f", 
                    1000.f * _MinDuration, 
                    1000.f * _Duration / _NbFrames, 
                    1000.f * _MaxDuration);
                ImGui::End();
            }

            // helper
            if(_ImGuiShowHelp){
                ImVec2 size{300, 400};
                ImVec2 pos{0, 0};
                ImGui::SetNextWindowPos(pos);
                ImGui::Begin("Help", &_ImGuiShowHelp);
                ImGui::SetWindowSize(size);
                ImGui::Text("Camera:\n" \
                            "  W: Go forward\n" \
                            "  A: Go left\n" \
                            "  S: Go back\n" \
                            "  D: Go right\n" \
                            "  Up: Go world up\n" \
                            "  Down: Go world down\n" \
                            "  Shift: Go faster\n" \
                            "  Mouse: Move camera\n" \
                );
                ImGui::Text("\nOther:\n" \
                            "  F: Enable wireframe\n" \
                            "  G: Toogle analytics\n" \
                            "  H: Toogle help\n" \
                            "  J: Enable\\Disable cursor\n" \
                            "  Escape: Quit\n" \
                );
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

    public:
        Application(){}

        void init();
        void run();
        void quit();

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

            if(app->_MouseMode != MOUSE_MODE_CAMERA) return;

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