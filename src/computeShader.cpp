#include "computeShader.hpp"
#include "errorHandler.hpp"

#include <fstream>
#include <sstream>
#include <GL/glext.h>

ComputeShader::ComputeShader(const std::string& shaderPath){
    _Id = glCreateProgram();

    const std::string code = openShaderFile(shaderPath);
    GLuint codeID = compile(code);
    link(codeID);

    deleteShader(codeID);
}

const std::string ComputeShader::openShaderFile(const std::string& path) const{
    std::string shaderCode;
    std::ifstream shaderFile;

    try {
        // open files
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();	
        // close file handlers
        shaderFile.close();
        // convert stream into string
        shaderCode = shaderStream.str();	
    }
    catch(std::ifstream::failure e){
        fprintf(stderr, "Failed to read the file: %s!\n", path.c_str());
        ErrorHandler::handle(IO_ERROR);
    }
    return shaderCode;
}

void ComputeShader::link(GLuint shader){
    glAttachShader(_Id, shader);
    int success;
    char infoLog[512];
    glLinkProgram(_Id);
    glGetProgramiv(_Id, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(_Id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to link the shaders:\n\t%s\n", infoLog);
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}

GLuint ComputeShader::compile(const std::string& code) const{
    int success;
    char infoLog[512];

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* codeCStr = code.c_str();
    glShaderSource(shader, 1, &codeCStr, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile the compute shader: \n\t%s!\n", infoLog);
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    };

    return shader;
}
