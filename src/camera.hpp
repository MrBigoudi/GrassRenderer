#pragma once

#include <glad/gl.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera{
    private:
        // camera Attributes
        glm::vec3 _Eye;
        glm::vec3 _At;
        glm::vec3 _Up;

        float _Fov = 0.f;
        float _AspectRatio = 0.f;
        float _Near = 0.f;
        float _Far = 0.f;

        float _MovementSpeed = 0.5f;
    
    public:
        bool _Accelerate = false;

    private:
        void updateEye(const glm::vec3& newPos){
            _Eye = newPos;
        }

        void updateAt(const glm::vec3& newLookAt){
            _At = newLookAt;
        }

        void updateUp(const glm::vec3& newWorldUp){
            _Up = newWorldUp;
        }

    public:
        // constructor with vectors
        Camera(
            float aspectRatio,
            float fov = 45.f,
            float near = 0.1f,
            float far = 100.f,
            const glm::vec3& position = glm::vec3(0.f, 1.f, 3.0f),
            const glm::vec3& lookAt = glm::vec3(),
            const glm::vec3& worldUp = glm::vec3(0.f, 1.f, 0.f)
        ){
            _AspectRatio = aspectRatio;
            _Fov = fov;
            _Near = near;
            _Far = far;
            updateEye(position);
            updateAt(lookAt);
            updateUp(worldUp);
        }

        glm::mat4 getView(){
            return glm::lookAt(_Eye, _At, _Up);
        }

        glm::mat4 getPerspective(){
            return glm::perspective(glm::radians(_Fov), _AspectRatio, _Near, _Far);
        }

        void processKeyboard(CameraMovement direction, float deltaTime)
        {
            float velocity = _MovementSpeed * deltaTime;
            if(_Accelerate) velocity *= 5.f;

            switch(direction){
                case FORWARD:
                    updateEye(_Eye - glm::vec3(0.f, 0.f, velocity));
                    updateAt(_At - glm::vec3(0.f, 0.f, velocity));
                    break;
                case BACKWARD:
                    updateEye(_Eye + glm::vec3(0.f, 0.f, velocity));
                    updateAt(_At + glm::vec3(0.f, 0.f, velocity));
                    break;
                case LEFT:
                    updateEye(_Eye - glm::vec3(velocity, 0.f, 0.f));
                    updateAt(_At - glm::vec3(velocity, 0.f, 0.f));
                    break;
                case RIGHT:
                    updateEye(_Eye + glm::vec3(velocity, 0.f, 0.f));
                    updateAt(_At + glm::vec3(velocity, 0.f, 0.f));
                    break;
                case UP:
                    updateEye(_Eye + glm::vec3(0.f, velocity, 0.f));
                    updateAt(_At + glm::vec3(0.f, velocity, 0.f));
                    break;
                case DOWN:
                    updateEye(_Eye - glm::vec3(0.f, velocity, 0.f));
                    updateAt(_At - glm::vec3(0.f, velocity, 0.f));
                    break;
            }
        }
};