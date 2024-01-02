#pragma once

#include "computeShader.hpp"
#include "shaders.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

enum GrassSizes{
    GRASS_POSITION_NB_ELEMENT = 3,
    GRASS_COLOR_NB_ELEMENT = 3,
    GRASS_FACING_NB_ELEMENT = 2,
    GRASS_HEIGHT_NB_ELEMENT = 1,
    GRASS_WIDTH_NB_ELEMENT = 1,
    GRASS_ROTATION_NB_ELEMENT = 1,
    GRASS_POSITION_BUFFER_ELEMENT_SIZE = 3*sizeof(float),
    GRASS_FACING_BUFFER_ELEMENT_SIZE = 2*sizeof(float),
    GRASS_HEIGHT_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_WIDTH_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_COLOR_BUFFER_ELEMENT_SIZE = 3*sizeof(float),
    GRASS_ROTATION_BUFFER_ELEMENT_SIZE = sizeof(float),
};

enum GrassLOD{
    GRASS_HIGH_LOD = 1,
    GRASS_LOW_LOD = 2,
};

class GrassTile{

    private:
        static GLuint _IdCounter;

    private:
        // GLuint _NbGrassBlades = 1024;
        GLuint _NbGrassBlades = 2048;
        GLuint _GridNbCols = 16;
        GLuint _GridNbLines = 16;
        // GLfloat _TileLength = 0.5f;
        GLuint _TileId = _IdCounter++;
        glm::vec2 _TilePos;
        GrassLOD _LOD; 

        // buffers compute shader
        GLuint _PositionBuffer;
        GLuint _FacingBuffer;
        GLuint _HeightBuffer;
        GLuint _WidthBuffer;
        GLuint _ColorBuffer;
        GLuint _RotationBuffer;
        ComputeShader* _ComputeShader = nullptr;

        // buffers vertex shader
        GLuint _VAO;

    private:
        void initBuffers();
        void initShader(const std::string& shaderPath);
        void updateRenderingBuffers();

    public:
        GrassTile(const glm::vec2& tilePos, 
                GrassLOD tileLOD = GRASS_LOW_LOD,
                const std::string& shaderPath = "shader/grassCompute.glsl");
        
        void dispatchComputeShader(GLuint tileWidth, GLuint tileHeight);
        void render(Shaders* shaders);
        void setLOD(GrassLOD newLOD){
            _LOD = newLOD;
        }
        glm::vec3 getPos(){
            glm::vec3 tmp;
            tmp.x = _TilePos.x;
            tmp.y = 0.f;
            tmp.z = _TilePos.y;
            return tmp;
        }
};


class Grass{
    private:
        GLuint _NbTiles = 4;
        GLuint _TileWidth = 16;
        GLuint _TileHeight = 16;
        GLuint _TileNbCols = 2;
        GLuint _TileNbLines = 2;
        float _RadiusHighLOD = 50.f;

        std::vector<GrassTile*> _Tiles;

    public:
        Grass();
        void render(Shaders* shaders);
        void update(float dt, const glm::vec3& cameraPosition);
};