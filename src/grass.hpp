#pragma once

#include "computeShader.hpp"
#include "shaders.hpp"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

enum GrassSizes{
    GRASS_POSITION_NB_ELEMENT = 3,
    GRASS_FACING_NB_ELEMENT = 2,
    GRASS_HEIGHT_NB_ELEMENT = 1,
    GRASS_WIDTH_NB_ELEMENT = 1,
    GRASS_POSITION_BUFFER_ELEMENT_SIZE = sizeof(glm::vec3),
    GRASS_FACING_BUFFER_ELEMENT_SIZE = sizeof(glm::vec2),
    GRASS_HEIGHT_BUFFER_ELEMENT_SIZE = sizeof(float),
    GRASS_WIDTH_BUFFER_ELEMENT_SIZE = sizeof(float),
};

class Grass{

    private:
        GLuint _NbGrassBlades = 1024;
        GLuint _TileWidth = 32;
        GLuint _TileHeight = 32;
        GLuint _TileLength = 1;

        // buffers compute shader
        GLuint _PositionBuffer;
        GLuint _FacingBuffer;
        GLuint _HeightBuffer;
        GLuint _WidthBuffer;
        ComputeShader* _ComputeShader = nullptr;

        // buffers vertex shader
        GLuint _VAO;

    private:
        void initBuffers();
        void initShader(const std::string& shaderPath);
        void updateRenderingBuffers();

    public:
        Grass(const std::string& shaderPath = "shader/grassCompute.glsl");
        
        void dispatchComputeShader();
        void render(Shaders* shaders);
};