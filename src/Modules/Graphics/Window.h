#ifndef H_MODULES_GRAPHICS_WINDOW_H
#define H_MODULES_GRAPHICS_WINDOW_H


// Project include files
#include "GLheaders.h"

// System include files

// Window class:
// Wrapper for the GLFWwindow that handles creation of the window and binds a
// context and a VAO to it.
class Window
{
    public:
        // Constructors
        Window
           (GLFWmonitor  *p_monitor,
            const char   *title);

        Window(int width_pix, int height_pix, const char *title);

        // Destructor
        ~Window();

        // Getter for the different properties
        int getWidth();
        int getHeight();
        float getAspectRatio();

        // Getter for the window
        GLFWwindow *getGlfwWindow();

    private:
        // Pointer to the window created by GLFW
        GLFWwindow *_p_window;

        // Window icon
        GLFWimage _icon;

        // Size of the window;
        int _width;
        int _height;

        // Aspect ratio of the window
        float _aspectRatio;

        // ---- METHODS ----
        // Set hints for GLFW
        void doHints();

};


#endif // H_MODULES_GRAPHICS_WINDOW_H
