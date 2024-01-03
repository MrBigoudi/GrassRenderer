#pragma once

#include "shaders.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <memory>


class Light;
using LightPointer = std::shared_ptr<Light>;

enum LightType{
    PointLight,
};

/**
 * A class representing a light
*/
class Light{

    private:
        /**
         * The light's position
        */
        glm::vec3 _Position = glm::vec3(1.0f);

        /**
         * The light's color
        */
        glm::vec3 _Color = glm::vec3(1.0f);

        /**
         * The light's type
        */
        LightType _Type = PointLight;

    public:
        glm::vec3 getPos() const {return _Position;}
        glm::vec3 getCol() const {return _Color;}

    public:
        /**
         * A basic constructor
         * @param type The type of light
         * @param pos The light's position
         * @param col The light's color
        */
        Light(LightType type, const glm::vec3& pos, const glm::vec3& col){
            _Type = type;
            _Position = pos;
            _Color = col;
        }

        /**
         * Send the light to the gpu
         * @param shader The shader to use
         * @param name The name of the light
        */
        void sendGPU(const ShadersPointer& shader, const std::string& name){
            shader->setVec3f(name + ".position", _Position);
            shader->setVec3f(name + ".color", _Color);
        }

        /**
         * Get the type of the light
         * @return The type
        */
        LightType getType() const {
            return _Type;
        }
};
