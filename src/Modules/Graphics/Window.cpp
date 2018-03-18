// Self includes
#include "Modules/Graphics/Window.h"

// Project's includes
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "SOIL.h"

// System includes
#include <stdlib.h>


Window::Window
   (GLFWmonitor *p_monitor,
    const char  *title)
{
    // Set hints of GLFW
    doHints();

    // Retrieve video mode
    const GLFWvidmode *p_videoMode = glfwGetVideoMode(p_monitor);
    if(p_videoMode == NULL)
    {
        GLOG_ERR("Unable to retrieve video mode");
        return;
    }

    GLOG_MSG("Creating fullscreen window in main monitor: %dx%d",
        p_videoMode->width, p_videoMode->height);

    // Create window
    _p_window = glfwCreateWindow
       (p_videoMode->width, p_videoMode->height,
        title,
        p_monitor, NULL);
    if(_p_window == NULL)
    {
        GLOG_ERR("Unable to create GLFW window");
        return;
    }

    GLOG_MSG("Succesfully created window");

    glfwMakeContextCurrent(_p_window);

    // Store window properties
    _width = p_videoMode->width;
    _height = p_videoMode->height;
    _aspectRatio = ((float) _width) / ((float) _height);
}

Window::Window(int width_pix, int height_pix, const char *title)
{
    // Set hints for GLFW
    doHints();

    // Create window
    GLOG_MSG("Creating window: %dx%d",
        width_pix, height_pix);

    _p_window = glfwCreateWindow
       (width_pix, height_pix, title,
        NULL, NULL);
    if(_p_window == NULL)
    {
        GLOG_ERR("Unable to create GLFW window");
        return;
    }

    GLOG_MSG("Succesfully created window");

    // Set OpenGL context
    glfwMakeContextCurrent(_p_window);

    // Store window properties
    _width = width_pix;
    _height = height_pix;
    _aspectRatio = ((float) _width) / ((float) _height);
}

Window::~Window()
{
    // Zero variables
    _width = 0;
    _height = 0;
    _aspectRatio = 0.0f;

    // Destroy window
    glfwDestroyWindow(_p_window);
    _p_window = NULL;
}

void Window::doHints()
{
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef APPLE_PLATFORM
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

}

int Window::getWidth()
{
    return _width;
}

int Window::getHeight()
{
    return _height;
}

float Window::getAspectRatio()
{
    return _aspectRatio;
}

GLFWwindow *Window::getGlfwWindow()
{
    return _p_window;
}
