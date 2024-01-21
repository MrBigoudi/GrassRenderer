#include "grass.hpp"
#include "application.hpp"
#include "computeShader.hpp"
#include "errorHandler.hpp"
#include "frustum.hpp"
#include "material.hpp"
#include "shaders.hpp"
#include "utils.hpp"
#include <GL/glu.h>
#include <cstdlib>
#include <array>

void Grass::initBuffers(){
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
        GRASS_POSITION_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS,
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _PositionBuffer);

    // heights
    glNamedBufferStorage(_HeightBuffer, 
        GRASS_HEIGHT_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _HeightBuffer);

    // widths
    glNamedBufferStorage(_WidthBuffer, 
        GRASS_WIDTH_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _WidthBuffer);

    // colors
    glNamedBufferStorage(_ColorBuffer, 
        GRASS_COLOR_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS,
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _ColorBuffer);

    // rotations
    glNamedBufferStorage(_RotationBuffer, 
        GRASS_ROTATION_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _RotationBuffer);

    // tilt
    glNamedBufferStorage(_TiltBuffer, 
        GRASS_TILT_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS, 
        nullptr, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _TiltBuffer);

    // bend
    glNamedBufferStorage(_BendBuffer, 
        GRASS_BEND_BUFFER_ELEMENT_SIZE * _MAX_NB_GRASS_BLADES * _NB_PARALLEL_BUFFERS, 
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
    GLuint tileWidth, GLuint tileHeight,
    GrassLOD tileLOD,
    const std::string& shaderPath){
    // initBuffers();
    initShader(shaderPath);
    // updateRenderingBuffers();
    _TilePos = tilePos;
    _LOD = tileLOD;
    _TileHeight = tileHeight;
    _TileWidth = tileWidth;
}


void GrassTile::dispatchComputeShader(int parallelId, GLuint vao){
    // Bind the compute shader program
    auto& shader = _ComputeShader;
    shader->use();
    glBindVertexArray(vao);

    // _ComputeShader->setInt("nbGrassBlades", _NbGrassBlades);
    shader->setInt("tileWidth", _TileWidth);
    shader->setInt("tileHeight", _TileHeight);
    shader->setInt("gridNbCols", _GridNbCols);
    shader->setInt("gridNbLines", _GridNbLines);
    shader->setVec2f("tilePos", _TilePos);
    shader->setInt("tileID", (int)_TileId);
    shader->setInt("parallelId", parallelId);
    shader->setInt("nbBladesPerTile", _MAX_NB_GRASS_BLADES);

    // Dispatch the compute shader

    // Set your initial dispatch values
    int dispatchX = _NbGrassBlades;
    int dispatchY = 1;
    int dispatchZ = 1;

    // Adjust dispatch values based on maximum work group counts
    if (dispatchX > _MaxWorkGroupCountX) {
        dispatchY = (dispatchX + _MaxWorkGroupCountX - 1) / _MaxWorkGroupCountX;
        dispatchX = _MaxWorkGroupCountX;
    }

    if (dispatchY > _MaxWorkGroupCountY) {
        dispatchZ = (dispatchY + _MaxWorkGroupCountY - 1) / _MaxWorkGroupCountY;
        dispatchY = _MaxWorkGroupCountY;
    }

    if (dispatchZ > _MaxWorkGroupCountZ) {
        fprintf(stderr, "Too many grass blades per tile !\n");
        ErrorHandler::handle(GL_ERROR);
    }

    // Dispatch the compute shader
    glDispatchCompute(dispatchX, dispatchY, dispatchZ);
    // glDispatchCompute(_NbGrassBlades, 1, 1);
    // glMemoryBarrier(GL_ALL_BARRIER_BITS);
    // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // printBuffers();
}

void Grass::updateRenderingBuffers(){
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

void GrassTile::render(Shaders* shaders, float time, GLuint vao){
    // Bind the vertex array object and draw
    shaders->use();
    glBindVertexArray(vao);
    shaders->setInt("tileLOD", _LOD);
    shaders->setFloat("time", time);
    // shaders->setInt("nbParallelBuffers", _NbParallelBuffers);
    // shaders->setInt("nbBladesPerTile", _NbGrassBlades);
    // shaders->setInt("tileWidth", tileWidth);
    // shaders->setInt("tileHeight", tileHeight);
    // glDrawArraysInstanced(GL_POINTS, 0, 1, _NbGrassBlades);
    glDrawArrays(GL_POINTS, 0, _NbGrassBlades);
}

// void GrassTile::render(Shaders* shaders, float time, int nbGrassBlades, GLuint vao){
//     // Bind the vertex array object and draw
//     shaders->use();
//     glBindVertexArray(vao);
//     shaders->setInt("tileLOD", _LOD);
//     shaders->setFloat("time", time);
//     // shaders->setInt("parallelId", parallelId);
//     // shaders->setInt("nbBladesPerTile", _MAX_NB_GRASS_BLADES);
//     // glDrawArrays(GL_POINTS, 0, _NbGrassBlades);
//     glDrawArrays(GL_POINTS, 0, nbGrassBlades);
// }

GLuint GrassTile::_IdCounter = 1;
GLint GrassTile::_MaxWorkGroupCountX = 0;
GLint GrassTile::_MaxWorkGroupCountY = 0;
GLint GrassTile::_MaxWorkGroupCountZ = 0;

Grass::Grass(){
    glm::vec2 curPos = glm::vec2();
    _Material = MaterialPointer(new Material());
    GLuint nbTiles = _NbTileLength*_NbTileLength;

    initBuffers();
    updateRenderingBuffers();

    initLightShader();

    float totalWidth = _NbTileLength * _TileWidth;
    curPos.x = 0.f;
    curPos.y = -1.f * _TileHeight;
    for(GLuint i = 0; i < nbTiles; i++){
        bool endOfLine = (i % _NbTileLength == 0);
        curPos.x += 1.f * _TileWidth;
        if (endOfLine) {
            curPos.x = 0.f;
            curPos.y += 1.f * _TileHeight;
        }
        // std::cout << "pos: " << curPos.x << ", " << curPos.y << std::endl;
        _Tiles.push_back(new GrassTile(curPos, _TileWidth, _TileHeight));
    }

    // get max work group values
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &GrassTile::_MaxWorkGroupCountX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &GrassTile::_MaxWorkGroupCountY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &GrassTile::_MaxWorkGroupCountZ);
}

void Grass::renderBatch(Shaders* shaders, float time, 
    const std::array<GrassLOD, _NB_PARALLEL_BUFFERS>& lods, 
    const std::array<int, _NB_PARALLEL_BUFFERS>&  nbBlades
    ){
    shaders->use();
    glBindVertexArray(_VAO);
    shaders->setFloat("time", time);
    // shaders->setInt("nbBlades0", nbBlades[0]);
    // shaders->setInt("nbBlades1", nbBlades[1]);
    for(int i=0; i<_NB_PARALLEL_BUFFERS; i++){        
        int blades = nbBlades[i];
        int startId = i*_MAX_NB_GRASS_BLADES;
        if(blades == 0) continue;
        shaders->setInt("tileLOD", lods[i]);
        shaders->setInt("startId", startId);
        // shaders->setInt("nbBladesPerTile", _MAX_NB_GRASS_BLADES);
        glDrawArrays(GL_POINTS, 0, blades);
    }
}

void Grass::render(Shaders* shaders, const Camera* camera, const glm::mat4& view, const glm::mat4& proj){
    // Pass 1 - geometry
    glBindFramebuffer(GL_FRAMEBUFFER, _Gbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaders->setMat4f("proj", proj);
    shaders->setMat4f("view", view);

    glm::mat4 mvp = proj * view;
    // _Material->setShaderValues(shaders);
    Frustum frustum = camera->createFrustrum();

    // for(auto& tile : _Tiles){
    //     if(tile->shouldBeRendered(camera->getPosition(), frustum)){
    //     // if(true){
    //         tile->dispatchComputeShader(_ComputeShader);
    //         tile->render(shaders, _TotalTime, _VAO);
    //     }
    // }

    for(int i=0; i<_Tiles.size(); i+=_NB_PARALLEL_BUFFERS){
        std::array<int, _NB_PARALLEL_BUFFERS> nbBlades;
        std::array<GrassLOD, _NB_PARALLEL_BUFFERS> lods;
        bool shouldBeRendered = false;
        // #pragma omp parallel for
        for(int j = 0; j<_NB_PARALLEL_BUFFERS; j++){
            auto& tile = _Tiles[i+j];
            if(tile->shouldBeRendered(camera->getPosition(), frustum)){
                tile->dispatchComputeShader(j, _VAO);
                // tile->render(shaders, _TotalTime, j, _VAO);
                nbBlades[j] = (tile->_NbGrassBlades);
                lods[j] = (tile->_LOD);
                shouldBeRendered = true;
            } else {
                nbBlades[j] = 0;
                lods[j] = tile->_LOD;
            }
        }
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        if(shouldBeRendered){
            renderBatch(shaders, _TotalTime, lods, nbBlades);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Pass 2 - lighting
    lightShaderPass();
}

void Grass::update(float dt, const glm::vec3& cameraPosition){
    _TotalTime += dt;
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
            tile->setLOD(GRASS_HIGH_LOD, cameraPosition);
        } else {
            tile->setLOD(GRASS_LOW_LOD, cameraPosition);
        }
    }
}

void Grass::initBuffersLighting(){
    glGenFramebuffers(1, &_Gbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _Gbuffer);
    auto error = glGetError();

    // color + specular color buffer
    glGenTextures(1, &_TextureColorSpec);
    glBindTexture(GL_TEXTURE_2D, _TextureColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application::_Width, Application::_Height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _TextureColorSpec, 0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to init color texture !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    // position color buffer
    glGenTextures(1, &_TexturePosition);
    glBindTexture(GL_TEXTURE_2D, _TexturePosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application::_Width, Application::_Height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _TexturePosition, 0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to init position texture !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    // normal color buffer
    glGenTextures(1, &_TextureNormal);
    glBindTexture(GL_TEXTURE_2D, _TextureNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application::_Width, Application::_Height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _TextureNormal, 0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to init normal texture !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }


    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Application::_Width, Application::_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        fprintf(stderr, "Framebuffer not complete!\n");
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to init texture buffers !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    // draw quad
    float vertices[] = {
        -1.f, 1.f, 0.f,
        -1.f, -1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, -1.f, 0.f
    };
    float uvs[] = {
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f
    };
    int indices[] = {
        0,1,2,
        1,3, 2
    };
    GLuint vboVert, vboUvs, ibo;
    glCreateBuffers(1, &vboVert);
    glCreateBuffers(1, &vboUvs);
    glNamedBufferStorage(vboVert, sizeof(float)*12, vertices, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(vboUvs, sizeof(float)*8, vertices, GL_DYNAMIC_STORAGE_BIT);

    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, sizeof(int)*6, indices, GL_DYNAMIC_STORAGE_BIT);

    glCreateVertexArrays(1, &_LightVAO);

    glVertexArrayVertexBuffer(_LightVAO, 0, vboVert, 0, 3*sizeof(float));
    glVertexArrayVertexBuffer(_LightVAO, 1, vboUvs, 0, 2*sizeof(float));
    glVertexArrayElementBuffer(_LightVAO, ibo);

    glEnableVertexArrayAttrib(_LightVAO, 0);
    glEnableVertexArrayAttrib(_LightVAO, 1);

    glVertexArrayAttribFormat(_LightVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(_LightVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(_LightVAO, 0, 0);
    glVertexArrayAttribBinding(_LightVAO, 1, 0);

}

void Grass::lightShaderPass(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _LightShader->use();
    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to use light shader !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _TextureColorSpec);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _TexturePosition);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _TextureNormal);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to bind textures !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    // draw quad
    renderQuad();
    error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to draw quad !\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }

    // copy depth buffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _Gbuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, Application::_Width, Application::_Height, 0, 0, Application::_Width, Application::_Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}