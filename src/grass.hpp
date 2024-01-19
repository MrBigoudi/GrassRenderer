#pragma once

#include "camera.hpp"
#include "computeShader.hpp"
#include "frustum.hpp"
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

const GLuint NB_PARALLEL_BUFFERS = 2;


class GrassTile{

    friend Grass;

    private:
        static GLuint _IdCounter;
        static GLint _MaxWorkGroupCountX, _MaxWorkGroupCountY, _MaxWorkGroupCountZ;
        const static GLuint _MAX_NB_GRASS_BLADES = 8192;
        // const static GLuint _MAX_NB_GRASS_BLADES = 4096;
        // const static GLuint _MIN_NB_GRASS_BLADES = 1024;
        const static GLuint _MIN_NB_GRASS_BLADES = 256;

    private:
        // GLuint _NbGrassBlades = 10;
        GLuint _NbGrassBlades = _MAX_NB_GRASS_BLADES;
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

        bool isTileCornerInFrontOfPlane(const Plane& plane, const glm::vec3& tileCorner){
            float result = plane.getSignedDistanceToPlane(tileCorner);
            return result >= 0;
        }

        bool isTileCornerInFrustum(const Frustum& frustum, const glm::vec3& tileCorner){
            bool isWithinFrustum = true;
            return 
                   isTileCornerInFrontOfPlane(frustum._BottomFace, tileCorner)
                && isTileCornerInFrontOfPlane(frustum._TopFace, tileCorner)
                && isTileCornerInFrontOfPlane(frustum._LeftFace, tileCorner)
                && isTileCornerInFrontOfPlane(frustum._RightFace, tileCorner)
                && isTileCornerInFrontOfPlane(frustum._FarFace, tileCorner)
                && isTileCornerInFrontOfPlane(frustum._NearFace, tileCorner);
        }

        bool isTileInFrustrum(const Frustum& frustum, const std::vector<glm::vec3>& corners){
            for(const auto& corner : corners){
                glm::vec3 cornerTmp = glm::vec3(corner.x, corner.y, corner.z);
                if(isTileCornerInFrustum(frustum, cornerTmp)){
                    return true;
                }
            }
            return false;
        }

    public:
        GrassTile(const glm::vec2& tilePos, GLuint tileWidth, GLuint tileHeight,
                GrassLOD tileLOD = GRASS_LOW_LOD,
                const std::string& shaderPath = "shader/grassCompute.glsl");
        
        void dispatchComputeShader();
        void render(Shaders* shaders, float time);

        void setNbBlades(const glm::vec3& cameraPosition){
            glm::vec3 projectedCameraPosition = glm::vec3(cameraPosition.x, 0.f, cameraPosition.z);
            glm::vec3 projectedTilePosition = getCenter();
            float dist = glm::distance(projectedCameraPosition, projectedTilePosition);

            if(dist > _RadiusRender){
                _NbGrassBlades = _MIN_NB_GRASS_BLADES;
                return;
            }
            float alpha = (dist/_RadiusRender);
            _NbGrassBlades = _MAX_NB_GRASS_BLADES * (1.f - alpha) + _MIN_NB_GRASS_BLADES * alpha;
            // std::cout << "alpha: " << alpha << ", nb b: " << _NbGrassBlades << ", pos: ";
            // printGlm(projectedTilePosition);
            // std::cout << ", cam pos: ";
            // printGlm(projectedCameraPosition);
            // std::cout << std::endl;
        }

        void setLOD(GrassLOD newLOD, const glm::vec3& cameraPosition){
            _LOD = newLOD;
            switch (_LOD) {
                case GRASS_HIGH_LOD:
                    setNbBlades(cameraPosition);
                    break;
                case GRASS_LOW_LOD:
                    setNbBlades(cameraPosition);
                    break;
            }
        }

        glm::vec3 getCenter() const {
            return getPos() + glm::vec3(_TileWidth >> 1, 0.f, _TileHeight >> 1);
        }

        glm::vec3 getPos() const {
            glm::vec3 tmp;
            tmp.x = _TilePos.x;
            tmp.y = 0.f;
            tmp.z = _TilePos.y;
            return tmp;
        }


        // bool shouldBeRendered(const glm::vec3& cameraPosition, const glm::mat4& mvp){
        bool shouldBeRendered(const glm::vec3& cameraPosition, const Frustum& frustum){
            // auto startTest = std::chrono::high_resolution_clock::now();
            
            std::vector<glm::vec3> corners = {
                getPos(), // upleft
                getPos() + glm::vec3(_TileWidth, 0.f, 0.f), // upright
                getPos() + glm::vec3(0.f, 0.f, _TileHeight), //downleft
                getPos() + glm::vec3(_TileWidth, 0.f, _TileHeight) //downright
            };

            bool withinCircle = doCircleRectangleIntersect(
                glm::vec3(cameraPosition.x, 0.f, cameraPosition.z), 
                _RadiusRender,
                corners[0], corners[1], corners[2], corners[3]
            );
            if(!withinCircle) return false;

            // if all 4 corners are behind the camera projected on z = 0, don't render
            bool inFrustrum = isTileInFrustrum(frustum, corners);

            // auto endTest = std::chrono::high_resolution_clock::now();
            // displayTime(startTest, endTest, "test");

            return inFrustrum;
            // return true;
        }
};


class Grass{
    private:
        GLuint _NbTileLength = 100;
        // GLuint _NbTileLength = 1;
        // GLuint _TileWidth = 16;
        GLuint _TileWidth = 4;
        // GLuint _TileHeight = 16;
        GLuint _TileHeight = 4;
        float _RadiusHighLOD = 20.f;

        MaterialPointer _Material = nullptr;
        std::vector<GrassTile*> _Tiles;
        float _TotalTime = 0.f;

    public:
        Grass();
        void render(Shaders* shaders, const Camera* camera, const glm::mat4& view, const glm::mat4& proj);
        void update(float dt, const glm::vec3& cameraPosition);

        glm::vec3 getCenter() const {
            float x = 0.5f * (_NbTileLength * _TileWidth);
            float z = 0.5f * (_NbTileLength * _TileHeight);
            float y = 1.f;
            return glm::vec3(x,y,z);
        }
};