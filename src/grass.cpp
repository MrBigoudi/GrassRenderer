#include "grass.hpp"
#include "computeShader.hpp"
#include <GL/glu.h>

void Grass::initBuffers(){
    // Generate buffer objects
    glCreateBuffers(1, &_PositionBuffer);
    glCreateBuffers(1, &_FacingBuffer);
    glCreateBuffers(1, &_HeightBuffer);
    glCreateBuffers(1, &_WidthBuffer);

    // Set up buffers
    // positions
    glNamedBufferData(_PositionBuffer, 
        GRASS_POSITION_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _PositionBuffer);

    // facings
    glNamedBufferData(_FacingBuffer, 
        GRASS_FACING_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _FacingBuffer);

    // heights
    glNamedBufferData(_HeightBuffer, 
        GRASS_HEIGHT_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _HeightBuffer);

    // widths
    glNamedBufferData(_WidthBuffer, 
        GRASS_WIDTH_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _WidthBuffer);
    
}

void Grass::initShader(const std::string& shaderPath){
    _ComputeShader = new ComputeShader(shaderPath);
}

Grass::Grass(const std::string& shaderPath){
    initBuffers();
    initShader(shaderPath);
    updateRenderingBuffers();
}


void Grass::dispatchComputeShader(){
    // Bind the compute shader program
    _ComputeShader->use();

    // _ComputeShader->setInt("nbGrassBlades", _NbGrassBlades);
    _ComputeShader->setInt("tileWidth", _TileWidth);
    _ComputeShader->setInt("tileHeight", _TileHeight);
    _ComputeShader->setInt("tileLength", _TileLength);

    // Dispatch the compute shader
    glDispatchCompute(_NbGrassBlades, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void Grass::updateRenderingBuffers(){
    glCreateVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    // positions
    glBindVertexBuffer(0, _PositionBuffer, 0, GRASS_POSITION_BUFFER_ELEMENT_SIZE);
    glVertexAttribBinding(0,0);
    glVertexAttribFormat(0, GRASS_POSITION_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    
    // facings
    glBindVertexBuffer(1, _FacingBuffer, 0, GRASS_FACING_BUFFER_ELEMENT_SIZE);
    glVertexAttribBinding(1,1);
    glVertexAttribFormat(1, GRASS_FACING_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);

    // heights
    glBindVertexBuffer(2, _HeightBuffer, 0, GRASS_HEIGHT_BUFFER_ELEMENT_SIZE);
    glVertexAttribBinding(2,2);
    glVertexAttribFormat(2, GRASS_HEIGHT_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);

    // widths
    glBindVertexBuffer(3, _WidthBuffer, 0, GRASS_WIDTH_BUFFER_ELEMENT_SIZE);
    glVertexAttribBinding(3,3);
    glVertexAttribFormat(3, GRASS_WIDTH_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Grass::render(Shaders* shaders){
    shaders->use();
    updateRenderingBuffers();
    // Bind the vertex array object and draw
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, _NbGrassBlades);
    glBindVertexArray(0);

    // Unbind the rendering shader
    glUseProgram(0);
}