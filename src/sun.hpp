#pragma once

#include "light.hpp"
#include "shaders.hpp"

#include <vector>

class Sun{

    private:
        ShadersPointer _Shader;
        float _Radius = 0.f;
        glm::vec3 _Position;
        glm::vec3 _Color;
        int _Resolution = 16;

        GLuint _VAO = 0;
        GLuint _EBO = 0;
        std::vector<GLfloat> _Vertices = {};
        std::vector<GLuint> _Indices = {};

    private:
        void initMesh(){
            const int resolution = _Resolution;
            const float PI = 3.1416f;
            const float stepPhi = PI / resolution;
            const float stepTheta = 2 * PI / resolution;
            float phi = 0.0f;
            int nbVertices = 0;
            // Generate vertices
            for (int i = 0; i <= resolution; i++) {
                phi += stepPhi;
                float theta = 0.0f;
                for (int j = 0; j <= resolution; j++) {
                    theta += stepTheta;

                    float x = _Radius*sin(theta)*sin(phi) + _Position.x;
                    float y = _Radius*cos(phi) + _Position.y;
                    float z = _Radius*cos(theta)*sin(phi) + _Position.z;

                    _Vertices.push_back(x);
                    _Vertices.push_back(y);
                    _Vertices.push_back(z);
                    nbVertices++;
                }
            }

            // Generate indices
            for (int i = 0; i < resolution; i++) {
                for (int j = 0; j < resolution; j++) {
                    GLuint first = i * (resolution + 1) + j;
                    GLuint second = first + resolution + 1;

                    _Indices.push_back(first);
                    _Indices.push_back(second);
                    _Indices.push_back(first + 1);

                    _Indices.push_back(second);
                    _Indices.push_back(second + 1);
                    _Indices.push_back(first + 1);
                }
            }
        }

        void initGPU(){
            // Create buffers
            GLuint vbo;
            glCreateBuffers(1, &vbo);
            glCreateBuffers(1, &_EBO);

            // Upload data to the buffers
            glNamedBufferData(vbo, sizeof(GLfloat) * _Vertices.size(), _Vertices.data(), GL_STATIC_DRAW);
            glNamedBufferData(_EBO, sizeof(GLuint) * _Indices.size(), _Indices.data(), GL_STATIC_DRAW);

            // Create vertex array
            glCreateVertexArrays(1, &_VAO);

            // Bind vertex buffer to the vertex array
            glVertexArrayVertexBuffer(_VAO, 0, vbo, 0, 3 * sizeof(GLfloat));
            glEnableVertexArrayAttrib(_VAO, 0);
            glVertexArrayAttribFormat(_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(_VAO, 0, 0);

            // Bind element buffer to the vertex array
            glVertexArrayElementBuffer(_VAO, _EBO);
        }

    public:
        Sun(const LightPointer& light, float radius = 1.f){
            // init attributes
            _Radius = radius;
            _Position = light->getPos();
            _Color = light->getCol();
            // init shader
            initMesh();
            initGPU();
            _Shader = ShadersPointer(
                new Shaders(
                    "shader/sunVert.glsl", 
                    "shader/sunFrag.glsl"
                )
            );
            _Shader->setVec3f("sunCol", _Color);
        }

        void render(const glm::mat4& view, const glm::mat4& proj){
            _Shader->use();
            _Shader->setMat4f("view", view);
            _Shader->setMat4f("proj", proj);
            glBindVertexArray(_VAO);
            glDrawElements(GL_TRIANGLES, _Indices.size(), GL_UNSIGNED_INT, 0);
        }

};