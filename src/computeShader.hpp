#pragma once

#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

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

    public:
        ComputeShader(const std::string& shaderPath);
        
        ~ComputeShader(){
            glDeleteShader(_Id);
        }

        void use() const {
            glUseProgram(_Id);
        }

};