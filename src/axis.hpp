#pragma once

#include "shaders.hpp"
#include <glm/glm.hpp>

class Axis{

    private:
        ShadersPointer _Shaders;

        float* _Data;
        GLuint _Vao;

    public:
        Axis(){
            _Shaders = ShadersPointer(new Shaders("shader/axisVert.glsl", "shader/axisFrag.glsl"));

            _Data = new float[18] {
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, // x
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, // y
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f  // z
            };

            GLuint vbo;
            glCreateVertexArrays(1, &_Vao);
            glCreateBuffers(1, &vbo);

            glNamedBufferData(vbo, 18*sizeof(float), _Data, GL_STATIC_DRAW);
            
            glEnableVertexArrayAttrib(_Vao, 0);
            glVertexArrayAttribFormat(_Vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(_Vao, 0, 0);

            glVertexArrayVertexBuffer(_Vao, 0, vbo, 0, 3*sizeof(float));
        
            auto error = glGetError();
            if (error != GL_NO_ERROR) {
                fprintf(stderr, "Failed to initialize the axis!\n\tOpenGL error: %s\n", gluErrorString(error));
                ErrorHandler::handle(ErrorCodes::GL_ERROR);
            }
        }

        void render(const glm::mat4& view, const glm::mat4& proj){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            glBindVertexArray(_Vao);
            glLineWidth(2.0f);

            _Shaders->use();
            _Shaders->setMat4f("view", view);
            _Shaders->setMat4f("proj", proj);

            _Shaders->setVec4f("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 0, 2);
            _Shaders->setVec4f("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 2, 2);
            _Shaders->setVec4f("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 4, 2);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

};