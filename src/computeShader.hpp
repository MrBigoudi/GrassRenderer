#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "errorHandler.hpp"
#include <glm/glm.hpp>
#include <GL/glu.h>

class ComputeShader{

    public:
        GLuint _Id;

    private:
        GLuint compile(const std::string& code) const;
        void link(GLuint shader);
        const std::string openShaderFile(const std::string& path) const;
        void deleteShader(GLuint shader) const {
            glDeleteShader(shader);
        }

        void checkError(const std::string& name) const{
            auto error = glGetError();
            if (error != GL_NO_ERROR) {
                fprintf(stderr, "Failed to set %s!\n\tOpenGL error: %s\n", name.c_str(), gluErrorString(error));
                ErrorHandler::handle(ErrorCodes::GL_ERROR);
            }
        }

        GLint getUniformLocation(const std::string& name) const {
            GLint uniformLocation = glGetUniformLocation(_Id, name.c_str());
            if (uniformLocation == -1) {
                fprintf(stderr, "Failed to set %s!\n\terror: Uniform not found!\n", name.c_str());
                ErrorHandler::handle(ErrorCodes::GL_ERROR);
            }
            return uniformLocation;
        }

    public:
        ComputeShader(const std::string& shaderPath);
        
        ~ComputeShader(){
            glDeleteShader(_Id);
        }

        void use() const {
            glUseProgram(_Id);
            auto error = glGetError();
            if (error != GL_NO_ERROR) {
                fprintf(stderr, "Failed to use the compute shader!\n\tOpenGL error: %s\n", gluErrorString(error));
                ErrorHandler::handle(ErrorCodes::GL_ERROR);
            }
        }

        /**
         * Set a uniform boolean
         * @param name The variable's name
         * @param val The variable's value
        */
        void setBool(const std::string& name, bool val) const {
            use();
            glUniform1i(getUniformLocation(name), (GLuint)val);
            checkError(name);
        }

        /**
         * Set a uniform int
         * @param name The variable's name
         * @param val The variable's value
        */
        void setInt(const std::string& name, int val) const {
            use();
            glUniform1i(getUniformLocation(name), val);
            checkError(name);
        }

        /**
         * Set a uniform float
         * @param name The variable's name
         * @param val The variable's value
        */
        void setFloat(const std::string& name, float val) const {
            use();
            glUniform1f(getUniformLocation(name), val);
            checkError(name);
        }

        /**
         * Set a uniform 4x4 float matrix
         * @param name The variable's name
         * @param val The variable's value
        */
        void setMat4f(const std::string& name, const glm::mat4x4& val) const {
            use();
            glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(val));
            checkError(name);
        }

        /**
         * Set a uniform 3x1 float vector
         * @param name The variable's name
         * @param val The variable's value
        */
        void setVec3f(const std::string& name, const glm::vec3& val) const {
            use();
            glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(val));
            checkError(name);
        }

        /**
         * Set a uniform 4x1 float vector
         * @param name The variable's name
         * @param val The variable's value
        */
        void setVec4f(const std::string& name, const glm::vec4& val) const {
            use();
            glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(val));
            checkError(name);
        }

};