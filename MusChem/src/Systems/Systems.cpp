#include "GLogger/GLogger.h"
#include "GLheaders.h"

#include "Systems/BasicModules.h"
#include "Systems/DebugModules.h"


Graphics graphics(1280, 1024, "Synth", "configs/graphics.conf");
Input input("configs/controls.conf");

bool renderPhysBoxes = false;
bool renderDialBoxes = false;
bool renderNames = true;

void checkOpenGLerror()
{
    GLenum error = glGetError();

    switch(error)
    {
        case GL_NO_ERROR:
            // Listed for completeness, don't log anything
            break;
        case GL_INVALID_ENUM:
            GLOG_ERR("GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE:
            GLOG_ERR("GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION:
            GLOG_ERR("GL_INVALID_OPERATION");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            GLOG_ERR("GL_INVALID_FRAMEBUFFER_OPERATION");
            break;
        case GL_OUT_OF_MEMORY:
            GLOG_ERR("GL_OUT_OF_MEMORY");
            break;
        default:
            GLOG_ERR("Unknown error (which is even scarier)");
    }
}
