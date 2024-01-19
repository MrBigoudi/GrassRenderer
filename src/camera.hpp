#pragma once

#include <glad/gl.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "frustum.hpp"

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
        glm::vec3 _WorldUp;

        glm::vec3 _Up;
        glm::vec3 _Right;

        float _Fov = 0.f;
        float _AspectRatio = 0.f;
        float _Near = 0.f;
        float _Far = 0.f;

        float _MovementSpeed = 0.5f;
        float _MouseSensitivity = 0.1f;

        float _Yaw = -90.f;
        float _Pitch = 0.f;
    
    public:
        bool _Accelerate = false;

    public:
        // constructor with vectors
        Camera(
            const glm::vec3& position,
            float aspectRatio,
            float fov = 45.f,
            float near = 0.1f,
            float far = 200.f,
            const glm::vec3& worldUp = glm::vec3(0.f, 1.f, 0.f)
        ){
            _AspectRatio = aspectRatio;
            _Fov = fov;
            _Near = near;
            _Far = far;
            _WorldUp = worldUp;
            _Eye = position;
            updateCameraVectors();
        }

        glm::vec3 getAt() const {
            return _At;
        }

        glm::vec3 getPosition() const {
            return _Eye;
        }

        glm::mat4 getView() const {
            return glm::lookAt(_Eye, _Eye + _At, _Up);
        }

        glm::mat4 getPerspective() const {
            return glm::perspective(glm::radians(_Fov), _AspectRatio, _Near, _Far);
        }

        void processKeyboard(CameraMovement direction, float deltaTime)
        {
            float velocity = _MovementSpeed * deltaTime;
            if(_Accelerate) velocity *= 5.f;

            switch(direction){
                case FORWARD:
                    _Eye += _At * velocity;
                    // _At  -= glm::vec3(0.f, 0.f, velocity);
                    break;
                case BACKWARD:
                    _Eye -= _At * velocity;
                    // _At  += glm::vec3(0.f, 0.f, velocity);
                    break;
                case LEFT:
                    _Eye -= _Right * velocity;
                    // _At  -= _Right * velocity;
                    break;
                case RIGHT:
                    _Eye += _Right * velocity;
                    // _At  += _Right * velocity;
                    break;
                case UP:
                    _Eye += _WorldUp * velocity;
                    // _At += _Up * velocity;
                    break;
                case DOWN:
                    _Eye -= _WorldUp * velocity;
                    // _At  -= _Up * velocity;
                    break;
            }
        }

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true){
            xoffset *= _MouseSensitivity;
            yoffset *= _MouseSensitivity;

            _Yaw   += xoffset;
            _Pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch){
                if (_Pitch > 89.0f)
                    _Pitch = 89.0f;
                if (_Pitch < -89.0f)
                    _Pitch = -89.0f;
            }

            // update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

    private:
        void updateCameraVectors(){
            // calculate the new at vector
            glm::vec3 front;
            front.x = cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
            front.y = sin(glm::radians(_Pitch));
            front.z = sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
            _At = glm::normalize(front);
            // also re-calculate the Right and Up vector
            _Right = glm::normalize(glm::cross(_At, _WorldUp));
            _Up    = glm::normalize(glm::cross(_Right, _At));
        }

    public:
        Frustum createFrustrum() const {
            Frustum frustum;
            const float halfVSide = _Far * tanf(_Fov * .5f);
            const float halfHSide = halfVSide * _AspectRatio;
            const glm::vec3 frontMultFar = _Far * _At;

            frustum._NearFace = { _Eye + _Near * _At, _At };
            frustum._FarFace = { _Eye + frontMultFar, -_At };
            frustum._RightFace = { _Eye,
                                    glm::cross(frontMultFar - _Right * halfHSide, _Up) };
            frustum._LeftFace = { _Eye,
                                    glm::cross(_Up,frontMultFar + _Right * halfHSide) };
            frustum._TopFace = { _Eye,
                                    glm::cross(_Right, frontMultFar - _Up * halfVSide) };
            frustum._BottomFace = { _Eye,
                                    glm::cross(frontMultFar + _Up * halfVSide, _Right) };

            return frustum;
        }
};