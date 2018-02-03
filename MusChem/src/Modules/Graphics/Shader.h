#ifndef H_MODULES_GRAPHICS_GL_PROGRAM_H
#define H_MODULES_GRAPHICS_GL_PROGRAM_H


// Project include files
#include "GLheaders.h"

// System include files
#include <string>

// Shader class:
// This class defines a compiled and linked OpenGL program from the shaders
// specified.
class Shader
{
    public:
        // Constructor
        Shader(std::string shaderDir);

        // Destructor
        ~Shader();

        // Activate program
        void activate();

        // Get program id
        GLuint getId();

    private:
        // ID for the program
        GLuint _programId;
};

#endif // H_MODULES_GRAPHICS_GL_PROGRAM_H
