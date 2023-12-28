#include "grass.hpp"
#include "computeShader.hpp"
#include <GL/glu.h>

void Grass::initBuffers(){
    glCreateVertexArrays(1, &_VAO);
    // Generate buffer objects
    glCreateBuffers(1, &_PositionBuffer);
    glCreateBuffers(1, &_FacingBuffer);
    glCreateBuffers(1, &_HeightBuffer);
    glCreateBuffers(1, &_WidthBuffer);

    // Set up buffers
    // positions
    glNamedBufferStorage(_PositionBuffer, 
        GRASS_POSITION_BUFFER_ELEMENT_SIZE * _NbGrassBlades,
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _PositionBuffer);

    // facings
    glNamedBufferStorage(_FacingBuffer, 
        GRASS_FACING_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _FacingBuffer);

    // heights
    glNamedBufferStorage(_HeightBuffer, 
        GRASS_HEIGHT_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _HeightBuffer);

    // widths
    glNamedBufferStorage(_WidthBuffer, 
        GRASS_WIDTH_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _WidthBuffer);

    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to initialize the grass buffers!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
    
}

void Grass::initShader(const std::string& shaderPath){
    _ComputeShader = new ComputeShader(shaderPath);

    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to initialize the grass shaders!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

Grass::Grass(const std::string& shaderPath){
    initBuffers();
    initShader(shaderPath);
    updateRenderingBuffers();
}


void Grass::dispatchComputeShader(){
    // Bind the compute shader program
    _ComputeShader->use();
    glBindVertexArray(_VAO);
    // _ComputeShader->setInt("nbGrassBlades", _NbGrassBlades);
    _ComputeShader->setInt("tileWidth", _TileWidth);
    _ComputeShader->setInt("tileHeight", _TileHeight);
    _ComputeShader->setFloat("tileLength", _TileLength);

    // Dispatch the compute shader
    glDispatchCompute(_NbGrassBlades, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void Grass::updateRenderingBuffers(){
    auto error = glGetError();

    // positions
    glEnableVertexArrayAttrib(_VAO, 0);
    glVertexArrayAttribFormat(_VAO, 0, GRASS_POSITION_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 0, 0);
    glVertexArrayVertexBuffer(_VAO, 0, _PositionBuffer, 0, GRASS_POSITION_BUFFER_ELEMENT_SIZE);

    // facings
    glEnableVertexArrayAttrib(_VAO, 1);
    glVertexArrayAttribFormat(_VAO, 1, GRASS_FACING_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 1,1);
    glVertexArrayVertexBuffer(_VAO, 1, _FacingBuffer, 0, GRASS_FACING_BUFFER_ELEMENT_SIZE);

    // heights
    glEnableVertexArrayAttrib(_VAO, 2);
    glVertexArrayAttribFormat(_VAO, 2, GRASS_HEIGHT_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 2,2);
    glVertexArrayVertexBuffer(_VAO, 2, _HeightBuffer, 0, GRASS_HEIGHT_BUFFER_ELEMENT_SIZE);

    // widths
    glEnableVertexArrayAttrib(_VAO, 3);
    glVertexArrayAttribFormat(_VAO, 3, GRASS_WIDTH_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 3,3);
    glVertexArrayVertexBuffer(_VAO, 3, _WidthBuffer, 0, GRASS_WIDTH_BUFFER_ELEMENT_SIZE);
    
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to update the grass buffers!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

void Grass::render(Shaders* shaders){
    // updateRenderingBuffers();
    // Bind the vertex array object and draw
    glBindVertexArray(_VAO);
    shaders->use();
    glDrawArrays(GL_POINTS, 0, _NbGrassBlades);
}