#pragma once

#include "computeShader.hpp"
#include "material.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include <glad/gl.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

enum GrassSizes{
    GRASS_POSITION_NB_ELEMENT = 4,
    GRASS_HEIGHT_NB_ELEMENT = 1,
    GRASS_WIDTH_NB_ELEMENT = 1,
    GRASS_COLOR_NB_ELEMENT = 4,
    GRASS_ROTATION_NB_ELEMENT = 1,
    GRASS_TILT_NB_ELEMENT = 1,
    GRASS_BEND_NB_ELEMENT = 2,
    GRASS_POSITION_BUFFER_ELEMENT_SIZE = 4*sizeof(float),
    GRASS_HEIGHT_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_WIDTH_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_COLOR_BUFFER_ELEMENT_SIZE = 4*sizeof(float),
    GRASS_ROTATION_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_TILT_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_BEND_BUFFER_ELEMENT_SIZE = 2*sizeof(float),
};

enum GrassLOD{
    GRASS_HIGH_LOD = 1,
    GRASS_LOW_LOD = 2,
};

class Grass;

class GrassTile{

    friend Grass;

    private:
        static GLuint _IdCounter;
        static GLint _MaxWorkGroupCountX, _MaxWorkGroupCountY, _MaxWorkGroupCountZ;
        const static GLuint _NB_GRASS_BLADES_HIGH_LOD = 4096;
        // const static GLuint _NB_GRASS_BLADES_HIGH_LOD = 256;
        const static GLuint _NB_GRASS_BLADES_LOW_LOD = 1024;

    private:
        // GLuint _NbGrassBlades = 10;
        GLuint _NbGrassBlades = _NB_GRASS_BLADES_HIGH_LOD;
        // GLuint _NbGrassBlades = 2 << 20;
        GLuint _GridNbCols = 16;
        GLuint _GridNbLines = 16;
        // GLfloat _TileLength = 0.5f;
        GLuint _TileId = _IdCounter++;
        glm::vec2 _TilePos;
        GLuint _TileHeight;
        GLuint _TileWidth;
        GrassLOD _LOD; 
        GLuint _RadiusRender = 30.f;

        GLuint _NbParallelBuffers = 10;

        // buffers compute shader
        GLuint _PositionBuffer;
        GLuint _HeightBuffer;
        GLuint _WidthBuffer;
        GLuint _ColorBuffer;
        GLuint _RotationBuffer;
        GLuint _TiltBuffer;
        GLuint _BendBuffer;
        ComputeShader* _ComputeShader = nullptr;

        // buffers vertex shader
        GLuint _VAO;

    private:
        void initBuffers();
        void initShader(const std::string& shaderPath);
        void updateRenderingBuffers();

        void checkBufferReadError(const std::string& bufferName) const {
            auto error = glGetError();
            if (error != GL_NO_ERROR) {
                fprintf(stderr, "Failed to display the buffer %s!\n\tOpenGL error: %s\n", 
                    bufferName.c_str(),
                    gluErrorString(error)
                );
                ErrorHandler::handle(ErrorCodes::GL_ERROR);
            }
        }

        void printFloatBuffer(GLuint buffer, const std::string& bufferName) const {
            std::vector<float> bufferData(_NbGrassBlades);
            glGetNamedBufferSubData(buffer, 0, _NbGrassBlades * sizeof(float), bufferData.data());
            checkBufferReadError(bufferName);
            std::cout << "Contents of buffer '" << bufferName << "':\n";
            for (GLuint i = 0; i < _NbGrassBlades; i++) {
                std::cout << bufferData[i] << " ";
            }
            std::cout << std::endl;
        }

        void printVec4Buffer(GLuint buffer, const std::string& bufferName) const {
            std::vector<float> bufferData(4*_NbGrassBlades);
            glGetNamedBufferSubData(buffer, 0, _NbGrassBlades * 4 * sizeof(float), bufferData.data());
            checkBufferReadError(bufferName);
            std::cout << "Contents of buffer '" << bufferName << "':\n";
            for (GLuint i = 0; i < 4*_NbGrassBlades; i+=4) {
                std::cout << "("
                    << bufferData[i] << ","
                    << bufferData[i+1] << ","
                    << bufferData[i+2] << ","
                    << bufferData[i+3] << ") ";
            }
            std::cout << std::endl;
        }

        void printVec2Buffer(GLuint buffer, const std::string& bufferName) const {
            std::vector<float> bufferData(2*_NbGrassBlades);
            glGetNamedBufferSubData(buffer, 0, _NbGrassBlades * 2 * sizeof(float), bufferData.data());
            checkBufferReadError(bufferName);
            std::cout << "Contents of buffer '" << bufferName << "':\n";
            for (GLuint i = 0; i < 2*_NbGrassBlades; i+=2) {
                std::cout << "("
                    << bufferData[i] << ","
                    << bufferData[i+1] << ") ";
            }
            std::cout << std::endl;
        }

        void printBuffers() const {
            printVec4Buffer(_PositionBuffer, "Position");
            printFloatBuffer(_HeightBuffer, "Height");
            printFloatBuffer(_HeightBuffer, "Width");
            printVec4Buffer(_ColorBuffer, "Color");
            printFloatBuffer(_RotationBuffer, "Rotation");
            printFloatBuffer(_TiltBuffer, "Tilt");
            printVec2Buffer(_BendBuffer, "Bend");
            exit(EXIT_SUCCESS);
        }

    public:
        GrassTile(const glm::vec2& tilePos, GLuint tileWidth, GLuint tileHeight,
                GrassLOD tileLOD = GRASS_LOW_LOD,
                const std::string& shaderPath = "shader/grassCompute.glsl");
        
        void dispatchComputeShader();
        void render(Shaders* shaders, float time);
        void setLOD(GrassLOD newLOD){
            _LOD = newLOD;
            switch (_LOD) {
                case GRASS_HIGH_LOD:
                    _NbGrassBlades = _NB_GRASS_BLADES_HIGH_LOD;
                    break;
                case GRASS_LOW_LOD:
                    _NbGrassBlades = _NB_GRASS_BLADES_LOW_LOD;
            }
        }
        glm::vec3 getPos(){
            glm::vec3 tmp;
            tmp.x = _TilePos.x;
            tmp.y = 0.f;
            tmp.z = _TilePos.y;
            return tmp;
        }

        bool shouldBeRendered(const glm::vec3& cameraPosition, const glm::mat4& mvp){
            // auto startTest = std::chrono::high_resolution_clock::now();
            
            std::vector<glm::vec4> corners = {
                glm::vec4(_TilePos.x, 0.f, _TilePos.y, 1.f),
                glm::vec4(_TilePos.x+_TileWidth, 0.f, _TilePos.y, 1.f),
                glm::vec4(_TilePos.x, 0.f, _TilePos.y+_TileHeight, 1.f),
                glm::vec4(_TilePos.x+_TileWidth, 0.f, _TilePos.y+_TileHeight, 1.f)
            };

            bool withinCircle = doCircleRectangleIntersect(
                glm::vec3(cameraPosition.x, 0.f, cameraPosition.z), 
                _RadiusRender,
                glm::vec3(corners[0].x, 0.f, corners[0].z),
                glm::vec3(corners[0].x, 0.f, corners[0].z),
                glm::vec3(corners[0].x, 0.f, corners[0].z),
                glm::vec3(corners[0].x, 0.f, corners[0].z)
            );
            if(!withinCircle) return false;

            // if all 4 corners are behind the camera projected on z = 0, don't render
            bool behindCam = true;
            for(int i=0; i<corners.size(); i++){
                float z = (mvp*corners[i]).z;
                if(z > 0.f){// in front
                    behindCam = false;
                    break;
                }
            }

            // auto endTest = std::chrono::high_resolution_clock::now();
            // displayTime(startTest, endTest, "test");

            return !behindCam;
            // return true;
        }
};


class Grass{
    private:
        GLuint _NbTileLength = 20;
        // GLuint _NbTileLength = 1;
        // GLuint _TileWidth = 16;
        GLuint _TileWidth = 4;
        // GLuint _TileHeight = 16;
        GLuint _TileHeight = 4;
        float _RadiusHighLOD = 20.f;

        GLuint _NbParallelBuffers = 10;

        MaterialPointer _Material = nullptr;
        std::vector<GrassTile*> _Tiles;
        float _TotalTime = 0.f;

    public:
        Grass();
        void render(Shaders* shaders, const glm::vec3& cameraPosition, const glm::mat4& view, const glm::mat4& proj);
        void update(float dt, const glm::vec3& cameraPosition);
};