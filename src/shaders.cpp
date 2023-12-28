#include "shaders.hpp"
#include <cstdlib>
#include <fstream>

Shaders::Shaders(const std::string& vert, const std::string& frag, const std::string& geom){
    _Id = glCreateProgram();
    checkID("Failed to init the program!\n");
    bool hasGeom = geom.compare("") != 0;

    const std::string vertCode = openShaderFile(vert);
    const std::string fragCode = openShaderFile(frag);
    const std::string geomCode = hasGeom ? openShaderFile(geom) : "";

    GLuint vertID = compileShader(vertCode, ShaderType::VERT);
    GLuint fragID = compileShader(fragCode, ShaderType::FRAG);
    GLuint geomID = hasGeom ? compileShader(geomCode, ShaderType::GEOM) : -1;

    linkShaders(vertID, fragID, geomID);

    deleteShader(vertID);
    deleteShader(fragID);
    if(hasGeom)deleteShader(geomID);
}

void Shaders::use() const {
    checkID("Can't use the shader before creating the program!\n");
    glUseProgram(_Id);
    auto error = glGetError();
    if (error != GL_NO_ERROR) {
        fprintf(stderr, "Failed to use the shader!\n\tOpenGL error: %s\n", gluErrorString(error));
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    }
}


const std::string Shaders::openShaderFile(const std::string& path) const{
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

GLuint Shaders::compileShader(const std::string& code, ShaderType type) const{
    GLuint shader = 0;
    std::string typeName = "";
    int success;
    char infoLog[512];

    switch(type){
        case VERT:
            shader = glCreateShader(GL_VERTEX_SHADER);
            typeName = "vertex";
            break;
        case FRAG:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            typeName = "fragment";
            break;
        case GEOM:
            shader = glCreateShader(GL_GEOMETRY_SHADER);
            typeName = "geometry";
            break;
    }

    const char* codeCStr = code.c_str();
    glShaderSource(shader, 1, &codeCStr, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile the %s shader: \n\t%s!\n", typeName.c_str(), infoLog);
        ErrorHandler::handle(ErrorCodes::GL_ERROR);
    };

    return shader;
}


void Shaders::linkShaders(GLuint vert, GLuint frag, GLuint geom){
    checkID("Can't link shaders before creating the program!\n");
    glAttachShader(_Id, vert);
    glAttachShader(_Id, frag);
    if(geom != -1) glAttachShader(_Id, geom);

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