#pragma once

#include <cstdio>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <ostream>
#include <vector>
#include <math.h>
#include <glm/glm.hpp>

#include "errorHandler.hpp"
#include "glm/geometric.hpp"

using Vertices = std::vector<GLfloat>;
using Colors = std::vector<GLfloat>;
using Uvs = std::vector<GLfloat>;
using Normals = std::vector<GLfloat>;

using Indices = std::vector<GLuint>;

using Vbo = std::vector<GLfloat>;

class Mesh;
using MeshPointer = std::shared_ptr<Mesh>;

enum VboType{
    VERTICES = 3,
    COLORS = 4,
    UVS = 2,
    NORMALS = 3
};

/**
 * A class that handle the mesh
*/
class Mesh{
    private:
        /**
         * The number of elements per vertices inside the vbo
        */
        static const GLuint _NB_ELEMENT_PER_VERTICES;

        /**
         * The vertex array object
        */
        GLuint _VAO = 0;

        /**
         * The vertex buffer object
        */
        GLuint _VBO = 0;

        /**
         * The element buffer object
        */
        GLuint _EBO = 0;

        /**
         * The number of vertices
        */
        GLuint _NbVertices = 0;

        /**
         * The vertex positions
        */
        Vertices _Vertices = {};

        /**
         * The vertex colors
        */
        Colors _Colors = {};

        /**
         * The vertex uvs
        */
        Uvs _Uvs = {};

        /**
         * The vertex normals
        */
        Normals _Normals = {};

        /**
         * The number of indices
        */
        GLuint _NbIndices = 0;

        /**
         * The indices
        */
        Indices _Indices = {};

        /**
         * The vbo
        */
        Vbo _VboData = {};

    private:
        /**
         * Create the vbo
         * @return The vbo as a floating point vector
        */
        void createVBO() {
            const GLuint size = _NbVertices*_NB_ELEMENT_PER_VERTICES;
            _VboData = Vbo(_NbVertices * _NB_ELEMENT_PER_VERTICES);

            for(int i=0; i<_NbVertices; i++){
                const GLuint idx   = i*_NB_ELEMENT_PER_VERTICES;
                const GLuint vIdx  = i*VboType::VERTICES;
                const GLuint cIdx  = i*VboType::COLORS;
                const GLuint uvIdx = i*VboType::UVS;
                const GLuint nIdx  = i*VboType::NORMALS;

                // set the vertices
                _VboData[idx]    = _Vertices[vIdx];
                _VboData[idx+1]  = _Vertices[vIdx+1];
                _VboData[idx+2]  = _Vertices[vIdx+2];
                // set the colors
                _VboData[idx+3]  = _Colors[cIdx];
                _VboData[idx+4]  = _Colors[cIdx+1];
                _VboData[idx+5]  = _Colors[cIdx+2];
                _VboData[idx+6]  = _Colors[cIdx+3];
                // set the uvs
                _VboData[idx+7]  = _Uvs[uvIdx];
                _VboData[idx+8]  = _Uvs[uvIdx+1];
                // set the normals
                _VboData[idx+9]  = _Normals[nIdx];
                _VboData[idx+10] = _Normals[nIdx+1];
                _VboData[idx+11] = _Normals[nIdx+2];
            }
        }

        /**
         * Bind the vbo and put the vertices data in it
        */
        void sendVBO() {
            if(_Vertices.empty()){
                fprintf(stderr, "Can't create GPU buffers without vertices!\n");
                ErrorHandler::handle(ErrorCodes::NOT_INITALIZED);
            }
            // create the vbo
            createVBO();

            // bind the vbo
            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, _VboData.size()*sizeof(GLfloat), _VboData.data(), GL_DYNAMIC_READ);
        }

        /**
         * Bind the ebo and put the indices data in it
        */
        void sendEBO() const {
            if(_NbIndices==0){
                fprintf(stderr, "Can't create GPU buffers without indices!\n");
                ErrorHandler::handle(ErrorCodes::NOT_INITALIZED);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _Indices.size()*sizeof(GLuint), _Indices.data(), GL_DYNAMIC_READ);
        }

        /**
         * Bind the vao and put the necessary data in it
        */
        void sendVAO() const {
            GLsizei size = _NB_ELEMENT_PER_VERTICES * sizeof(float);
            GLvoid* pointer = (void*)0;
            GLuint id = 0;
            // position attributes
            glVertexAttribPointer(id, VboType::VERTICES, GL_FLOAT, GL_FALSE, size, pointer);
            glEnableVertexAttribArray(id);
            // color attributes
            pointer = reinterpret_cast<GLvoid*>(VboType::VERTICES * sizeof(float));
            id++;
            glVertexAttribPointer(id, VboType::COLORS, GL_FLOAT, GL_FALSE, size, pointer);
            glEnableVertexAttribArray(id);
            // uv attributes
            pointer = reinterpret_cast<GLvoid*>((VboType::VERTICES + VboType::COLORS) * sizeof(float));
            id++;
            glVertexAttribPointer(id, VboType::UVS, GL_FLOAT, GL_FALSE, size, pointer);
            glEnableVertexAttribArray(id);
            // normals attributes
            pointer = reinterpret_cast<GLvoid*>((VboType::VERTICES + VboType::COLORS + VboType::UVS) * sizeof(float));
            id++;
            glVertexAttribPointer(id, VboType::NORMALS, GL_FLOAT, GL_FALSE, size, pointer);
            glEnableVertexAttribArray(id);
        }

    private:
        /**
         * An empty constructor
        */
        Mesh(){
            glGenVertexArrays(1, &_VAO);
            glGenBuffers(1, &_VBO);
            glGenBuffers(1, &_EBO);
        }


    public:

        /**
         * A constructor to initialize and bind everything
         * @param vertices The vertices (as x, y, z values)
         * @param indices The triangle indices
         * @param colors The vertices' colors (as r, g, b, a values)
         * @param uvs The vertices' uvs (as u, v values)
         * @param normals The vertices' normals (as nx, ny, nz values)
        */
        Mesh(Vertices vertices, Indices indices, Colors colors = {}, Uvs uvs = {}, Normals normals = {}) : Mesh() {
            // fill if empty lists
            if(vertices.empty()){
                fprintf(stderr, "Vertices can't be empty!\n");
                ErrorHandler::handle(ErrorCodes::OUT_OF_RANGE);
            }
            if(indices.empty()){
                fprintf(stderr, "Indices can't be empty!\n");
                ErrorHandler::handle(ErrorCodes::OUT_OF_RANGE);
            }
            _NbVertices = vertices.size() / VboType::VERTICES;
            if(colors.empty()) colors = Colors(_NbVertices*VboType::COLORS, 1.0f);
            if(uvs.empty()) uvs     = Uvs(_NbVertices*VboType::UVS, 0.0f);
            if(normals.empty()) normals = Normals(_NbVertices*VboType::NORMALS, 0.0f);

            // check sizes
            if(colors.size() != _NbVertices*VboType::COLORS){
                fprintf(stderr, "The number of colors should correspond to the number of vertices! Given %d, expected: %d!\n", (int)colors.size(), _NbVertices*VboType::COLORS);
                ErrorHandler::handle(ErrorCodes::OUT_OF_RANGE);
            }
            if(uvs.size() != _NbVertices*VboType::UVS){
                fprintf(stderr, "The number of uvs should correspond to the number of vertices! Given %d, expected: %d!\n", (int)uvs.size(), _NbVertices*VboType::UVS);
                ErrorHandler::handle(ErrorCodes::OUT_OF_RANGE);
            }
            if(normals.size() != _NbVertices*VboType::NORMALS){
                fprintf(stderr, "The number of normals should correspond to the number of vertices! Given %d, expected: %d!\n", (int)normals.size(), _NbVertices*VboType::NORMALS);
                ErrorHandler::handle(ErrorCodes::OUT_OF_RANGE);
            }

            _Vertices = vertices;
            _NbIndices = indices.size();
            _Indices = indices;
            _Colors = colors;
            _Uvs = uvs;
            _Normals = normals;
        }

        /**
         * A basic shallow copy constructor
         * @param mesh The mesh to copy
        */
        Mesh(const MeshPointer& mesh) : Mesh(){
            _NbVertices = mesh->_NbVertices;
            _Vertices = mesh->_Vertices;
            _NbIndices = mesh->_NbIndices;
            _Indices = mesh->_Indices;
            _Colors = mesh->_Colors;
            _Uvs = mesh->_Uvs;
            _Normals = mesh->_Normals;
        }

        /**
         * A basic destructor
        */
        ~Mesh(){
            glDeleteVertexArrays(1, &_VAO);
            glDeleteBuffers(1, &_VBO);
            glDeleteBuffers(1, &_EBO);
        }

        /**
         * Initiate the GPU geometry
        */
        void initGpuGeometry() {
            glBindVertexArray(_VAO);
            sendVBO();
            sendEBO();
            sendVAO();
        }

        /**
         * Render the mesh
        */
        void render() const {
            glBindVertexArray(_VAO);
            glDrawElements(GL_TRIANGLES, _NbIndices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }


        /**
         * Display the mesh
         * @param stream TO stream in which to display the mesh
        */
        void print(std::ostream& stream) const {
            stream << "Vertices:\n";
            for(int i=0; i<_NbVertices; i++){
                stream << _Vertices[VERTICES*i] << " " << _Vertices[VERTICES*i+1] << " " << _Vertices[VERTICES*i+2] << "\n";
            }

            stream << "\nIndices:\n";
            for(int i=0; i<_NbIndices; i+=3){
                stream << _Indices[i] << " " << _Indices[i+1] << " " << _Indices[i+2] << "\n";
            }

            stream << "\nColors:\n";
            for(int i=0; i<_NbVertices; i++){
                stream << _Colors[COLORS*i] << " " << _Colors[COLORS*i+1] << " " << _Colors[COLORS*i+2] << " " << _Colors[COLORS*i+3] << "\n";
            }

            stream << "\nUvs:\n";
            for(int i=0; i<_NbVertices; i++){
                stream << _Uvs[UVS*i] << " " << _Uvs[UVS*i+1] << "\n";
            }

            stream << "\nNormals:\n";
            for(int i=0; i<_NbVertices; i++){
                stream << _Normals[NORMALS*i] << " " << _Normals[NORMALS*i+1] << " " << _Normals[NORMALS*i+2] << "\n";
            }

            stream << std::endl;
        }

        /**
         * Set a unique color for all the vertices
         * @param color The color to set
        */
        void setSimpleColor(glm::vec4 color){
            Colors newColors = Colors(_Colors.size());
            for(int i=0; i<_NbVertices; i++){
                newColors[i*VboType::COLORS]   = color.r;
                newColors[i*VboType::COLORS+1] = color.g;
                newColors[i*VboType::COLORS+2] = color.b;
                newColors[i*VboType::COLORS+3] = color.a;
            }
            _Colors = newColors;
        }

};