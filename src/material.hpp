#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "errorHandler.hpp"
#include "shaders.hpp"


class Material;
using MaterialPointer = std::shared_ptr<Material>;

/**
 * A class representing a material
*/
class Material{

    private:
        /**
         * The ambient local illumination
        */
        GLfloat _Ambient = 0.05f;
        
        /**
         * The diffuse property
        */
        GLfloat _Diffuse = 1.f;

        /**
         * The specular property
        */
        GLfloat _Specular = 1.f;

        /**
         * The shininess
        */
        GLfloat _Shininess = 32.0f;


    public:
        /**
         * An empty constructor
        */
        Material(){}

        /**
         * A copy constructor
         * @param mat The material to copy
        */
        Material(const MaterialPointer& mat){
            _Ambient = mat->_Ambient;
            _Diffuse = mat->_Diffuse;
            _Specular = mat->_Specular;
            _Shininess = mat->_Shininess;
        }

        /**
         * A basic constructor
         * @param ambient The ambient property
         * @param diffuse The diffuse property
         * @param specular The specular property
         * @param shininess The shininess property
        */
        Material(GLfloat ambient, GLfloat diffuse, GLfloat specular, GLfloat shininess){
            _Ambient = ambient;
            _Diffuse = diffuse;
            _Specular = specular;
            _Shininess = shininess;
        }

        /**
         * Set the values for a given shader
         * @param shader The shader to use
         * @cond Shader must have the correct "fAmbient", "fDiffuse", "fSpecular" and "fShininess" variables
        */
        void setShaderValues(Shaders* shader){
            shader->use();
            shader->setFloat("fAmbient", _Ambient);
            shader->setFloat("fDiffuse", _Diffuse);
            shader->setFloat("fSpecular", _Specular);
            shader->setFloat("fShininess", _Shininess);
        }

        /**
         * Set the ambient value
         * @param ambient The ambient value
        */
        void setAmbient(GLfloat ambient){
            if(ambient < 0){
                fprintf(stderr, "Can't set a negative ambient value!\n");
                ErrorHandler::handle(ErrorCodes::BAD_VALUE, ErrorLevel::WARNING);
                return;
            }
            _Ambient = ambient;
        }
};