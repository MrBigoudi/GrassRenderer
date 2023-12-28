#pragma once

#include "shaders.hpp"
#include <glm/glm.hpp>

class Axis{

    private:
        ShadersPointer _Shaders;

        glm::mat4 _View;
        glm::mat4 _Proj;

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

            _Shaders->use();
            GLuint vbo;
            glCreateVertexArrays(1, &_Vao);
            glCreateBuffers(1, &vbo);

            glNamedBufferData(vbo, 18*sizeof(float), _Data, GL_STATIC_DRAW);
            
            glEnableVertexArrayAttrib(_Vao, 0);
            glVertexArrayAttribFormat(_Vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(_Vao, 0, 0);

            glVertexArrayVertexBuffer(_Vao, 0, vbo, 0, 3*sizeof(float));
        }

        void setMatrices(const glm::mat4& view, const glm::mat4& proj){
            _Proj = proj;
            _View = view;
        }

        void render(){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            glBindVertexArray(_Vao);
            glLineWidth(2.0f);

            _Shaders->use();
            _Shaders->setMat4f("view", _View);
            _Shaders->setMat4f("proj", _Proj);

            _Shaders->setVec4f("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 0, 2);
            _Shaders->setVec4f("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 2, 2);
            _Shaders->setVec4f("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
            glDrawArrays(GL_LINE_STRIP, 4, 2);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

};