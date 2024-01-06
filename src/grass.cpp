#include "grass.hpp"
#include "computeShader.hpp"
#include "material.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include <GL/glu.h>

void GrassTile::initBuffers(){
    glCreateVertexArrays(1, &_VAO);
    // Generate buffer objects
    glCreateBuffers(1, &_PositionBuffer);
    glCreateBuffers(1, &_HeightBuffer);
    glCreateBuffers(1, &_WidthBuffer);
    glCreateBuffers(1, &_ColorBuffer);
    glCreateBuffers(1, &_RotationBuffer);
    glCreateBuffers(1, &_TiltBuffer);
    glCreateBuffers(1, &_BendBuffer);

    // Set up buffers
    // positions
    glNamedBufferStorage(_PositionBuffer, 
        GRASS_POSITION_BUFFER_ELEMENT_SIZE * _NbGrassBlades,
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _PositionBuffer);

    // heights
    glNamedBufferStorage(_HeightBuffer, 
        GRASS_HEIGHT_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _HeightBuffer);

    // widths
    glNamedBufferStorage(_WidthBuffer, 
        GRASS_WIDTH_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _WidthBuffer);

    // colors
    glNamedBufferStorage(_ColorBuffer, 
        GRASS_COLOR_BUFFER_ELEMENT_SIZE * _NbGrassBlades,
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _ColorBuffer);

    // rotations
    glNamedBufferStorage(_RotationBuffer, 
        GRASS_ROTATION_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _RotationBuffer);

    // tilt
    glNamedBufferStorage(_TiltBuffer, 
        GRASS_TILT_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _TiltBuffer);

    // bend
    glNamedBufferStorage(_BendBuffer, 
        GRASS_BEND_BUFFER_ELEMENT_SIZE * _NbGrassBlades, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, _BendBuffer);

    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to initialize the grass buffers!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
    
}

void GrassTile::initShader(const std::string& shaderPath){
    _ComputeShader = new ComputeShader(shaderPath);

    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to initialize the grass shaders!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

GrassTile::GrassTile(
    const glm::vec2& tilePos,
    GrassLOD tileLOD, 
    const std::string& shaderPath){
    initBuffers();
    initShader(shaderPath);
    updateRenderingBuffers();
    _TilePos = tilePos;
    _LOD = tileLOD;
}


void GrassTile::dispatchComputeShader(GLuint tileWidth, GLuint tileHeight){
    // Bind the compute shader program
    _ComputeShader->use();
    glBindVertexArray(_VAO);

    _ComputeShader->setInt("nbGrassBlades", _NbGrassBlades);
    // _ComputeShader->setFloat("tileLength", _TileLength);
    _ComputeShader->setInt("tileWidth", tileWidth);
    _ComputeShader->setInt("tileHeight", tileHeight);
    _ComputeShader->setInt("gridNbCols", _GridNbCols);
    _ComputeShader->setInt("gridNbLines", _GridNbLines);
    _ComputeShader->setVec2f("tilePos", _TilePos);
    _ComputeShader->setInt("tileID", (int)_TileId);

    // Dispatch the compute shader
    glDispatchCompute(_NbGrassBlades, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void GrassTile::updateRenderingBuffers(){
    auto error = glGetError();

    // positions
    glEnableVertexArrayAttrib(_VAO, 0);
    glVertexArrayAttribFormat(_VAO, 0, GRASS_POSITION_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 0, 0);
    glVertexArrayVertexBuffer(_VAO, 0, _PositionBuffer, 0, GRASS_POSITION_BUFFER_ELEMENT_SIZE);

    // heights
    glEnableVertexArrayAttrib(_VAO, 1);
    glVertexArrayAttribFormat(_VAO, 1, GRASS_HEIGHT_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 1, 1);
    glVertexArrayVertexBuffer(_VAO, 1, _HeightBuffer, 0, GRASS_HEIGHT_BUFFER_ELEMENT_SIZE);

    // widths
    glEnableVertexArrayAttrib(_VAO, 2);
    glVertexArrayAttribFormat(_VAO, 2, GRASS_WIDTH_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 2, 2);
    glVertexArrayVertexBuffer(_VAO, 2, _WidthBuffer, 0, GRASS_WIDTH_BUFFER_ELEMENT_SIZE);

    // colors
    glEnableVertexArrayAttrib(_VAO, 3);
    glVertexArrayAttribFormat(_VAO, 3, GRASS_COLOR_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 3, 3);
    glVertexArrayVertexBuffer(_VAO, 3, _ColorBuffer, 0, GRASS_COLOR_BUFFER_ELEMENT_SIZE);

    // rotations
    glEnableVertexArrayAttrib(_VAO, 4);
    glVertexArrayAttribFormat(_VAO, 4, GRASS_ROTATION_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 4, 4);
    glVertexArrayVertexBuffer(_VAO, 4, _RotationBuffer, 0, GRASS_ROTATION_BUFFER_ELEMENT_SIZE);

    // tilt
    glEnableVertexArrayAttrib(_VAO, 5);
    glVertexArrayAttribFormat(_VAO, 5, GRASS_TILT_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 5, 5);
    glVertexArrayVertexBuffer(_VAO, 5, _TiltBuffer, 0, GRASS_TILT_BUFFER_ELEMENT_SIZE);

    // bend
    glEnableVertexArrayAttrib(_VAO, 6);
    glVertexArrayAttribFormat(_VAO, 6, GRASS_BEND_NB_ELEMENT, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(_VAO, 6, 6);
    glVertexArrayVertexBuffer(_VAO, 6, _BendBuffer, 0, GRASS_BEND_BUFFER_ELEMENT_SIZE);
    
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to update the grass buffers!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

void GrassTile::render(Shaders* shaders){
    // updateRenderingBuffers();
    // Bind the vertex array object and draw
    glBindVertexArray(_VAO);
    shaders->use();
    shaders->setInt("tileLOD", _LOD);
    glDrawArrays(GL_POINTS, 0, _NbGrassBlades);
}

GLuint GrassTile::_IdCounter = 1;

Grass::Grass(){
    glm::vec2 curPos = glm::vec2();
    _Material = MaterialPointer(new Material());
    for(GLuint i = 0; i<_NbTiles; i++){
        curPos.x = (i % _TileNbCols) * _TileWidth;
        curPos.y = (i / _TileNbLines) * _TileHeight;
        // GrassLOD lod = i == 0 ? GRASS_HIGH_LOD : GRASS_LOW_LOD;
        // GrassLOD lod = GRASS_HIGH_LOD;
        _Tiles.push_back(new GrassTile(curPos));
        // std::cout << "tile id: " << _Tiles[_Tiles.size()-1]->_TileId << std::endl;
    }
}

void Grass::render(Shaders* shaders){
    // _Material->setShaderValues(shaders);
    for(auto& tile : _Tiles){
        tile->dispatchComputeShader(_TileWidth, _TileHeight);
        tile->render(shaders);
    }
}

void Grass::update(float dt, const glm::vec3& cameraPosition){
    // update tiles lod
    for(auto& tile : _Tiles){
        // get tile's position
        glm::vec3 tileUpLeft = tile->getPos();
        glm::vec3 tileUpRight = tile->getPos() + glm::vec3(_TileWidth, 0.f, 0.f);
        glm::vec3 tileDownLeft = tile->getPos() + glm::vec3(0.f, 0.f, _TileHeight);
        glm::vec3 tileDownRight = tile->getPos() + glm::vec3(_TileWidth, 0.f, _TileHeight);
        // check if boundig box within camera radius
        if(doCircleRectangleIntersect(cameraPosition, _RadiusHighLOD, 
            tileUpLeft, tileUpRight, tileDownLeft, tileDownRight)){
            tile->setLOD(GRASS_HIGH_LOD);
        } else {
            tile->setLOD(GRASS_LOW_LOD);
        }
    }
}