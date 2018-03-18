// Self include files
#include "Modules/Graphics/Shader.h"

// Project include files
#include "GLogger/GLogger.h"

// System include files
#include <fstream>
#include <string>
#include <stdlib.h>


Shader::Shader(std::string shaderDir)
{
    // ---- FILE NAMES ----
    // The files must be named vertex.glsl and fragment.glsl
    std::string pathToVertexShader = shaderDir + "/vertex.glsl";
    std::string pathToFragmentShader = shaderDir + "/fragment.glsl";

    //GLOG_MSG("Loading shader program at %s", shaderDir.c_str());

    // ---- LOAD VERTEX SHADER ----
    std::ifstream vertexShaderStream(pathToVertexShader.c_str(), std::ios::in);
    std::string vertexShaderCode;

    // Read code if the file could be opened, fail if not
    if(vertexShaderStream.is_open())
    {
        std::string line = "";
        while(getline(vertexShaderStream, line))
        {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    }
    else
    {
        GLOG_ERR("Could not open vertex shader %s", pathToVertexShader.c_str());
        return;
    }

    // ---- LOAD FRAGMENT SHADER ----
    std::ifstream fragmentShaderStream(pathToFragmentShader.c_str(), std::ios::in);
    std::string fragmentShaderCode;

    // Read code if the file could be opened, fail if not
    if(fragmentShaderStream.is_open())
    {
        std::string line = "";
        while(getline(fragmentShaderStream, line))
        {
            fragmentShaderCode += "\n" + line;
        }
        fragmentShaderStream.close();
    }
    else
    {
        GLOG_ERR("Could not open fragment shader %s", pathToFragmentShader.c_str());
        return;
    }

    // ---- CREATE SHADERS ----
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // ---- COMPILE AND LINK PROGRAM ----
    // Checking compilation errors
    GLint result = GL_FALSE;
    int infoLogLength;

    // Compile vertex shader
    //GLOG_MSG("Compiling vertex shader");
    char const *p_vertexSrcPointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderId, 1, &p_vertexSrcPointer, NULL);
    glCompileShader(vertexShaderId);

    // Check vertex shader compilation
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(result == GL_FALSE)
    {
        char *vertexShaderErrorMessage = (char *) malloc((infoLogLength + 1) * sizeof(char));
        glGetShaderInfoLog
           (vertexShaderId, infoLogLength, NULL, vertexShaderErrorMessage);
        GLOG_ERR("could not compile vertex shader: %s",
                vertexShaderErrorMessage);
        printf("%s\n", vertexShaderErrorMessage);
        free(vertexShaderErrorMessage);
        return;
    }

    // Compile fragment shader
    //GLOG_MSG("Compiling fragment shader");
    char const *p_fragmentSrcPointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderId, 1, &p_fragmentSrcPointer, NULL);
    glCompileShader(fragmentShaderId);

    // Check vertex shader compilation
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(result == GL_FALSE)
    {
        char *fragmentShaderErrorMessage = (char *) malloc((infoLogLength + 1) * sizeof(char));
        glGetShaderInfoLog
           (fragmentShaderId, infoLogLength,
            NULL, &fragmentShaderErrorMessage[0]);
        GLOG_ERR("could not compile fragment shader: %s",
                fragmentShaderErrorMessage);
        free(fragmentShaderErrorMessage);
        return;
    }

    // Link the program
    //GLOG_MSG("Linking program");
    _programId = glCreateProgram();
    glAttachShader(_programId, vertexShaderId);
    glAttachShader(_programId, fragmentShaderId);
    glLinkProgram(_programId);

    // Check program
    glGetProgramiv(_programId, GL_LINK_STATUS, &result);
    glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(result == GL_FALSE)
    {
        char *programErrorMessage = (char *)malloc((infoLogLength + 1) * sizeof(char));
		glGetProgramInfoLog
           (_programId, infoLogLength, NULL, &programErrorMessage[0]);
        GLOG_ERR("Could not link program: %s", programErrorMessage);
        free(programErrorMessage);
        return;
    }

    // Log success
    GLOG_MSG("Shader program %s created successfully", shaderDir.c_str());
}

Shader::~Shader()
{
    glDeleteProgram(_programId);
}

void Shader::activate()
{
    glUseProgram(_programId);
}

GLuint Shader::getId()
{
    return _programId;
}
