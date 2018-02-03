#ifndef H_MODULES_GRAPHICS_GRAPHICS_H
#define H_MODULES_GRAPHICS_GRAPHICS_H


// Project include files
#include "GLheaders.h"
#include "Modules/Graphics/Window.h"
#include "Modules/Graphics/Camera.h"
#include "Modules/Graphics/Shader.h"

#include "GList/GList.h"

// System include files

// Graphics class:
// This class defines the Graphics module which handles everything related to
// graphics: window creation, rendering, etc.
class Graphics
{
    public:
        // Constructor: initialises GLFW, opens a window and initialises GLEW
        Graphics(const char *title, const char *pathToFile);

        // Constructor: initialises GLFW, opens a window and initialises GLEW
        Graphics
           (int width_pix, int height_pix,
            const char *title, const char *pathToFile);

        // Destructor: destroys the window opened
        ~Graphics();

        // This function clears the screen
        void beginRender();

        // This function swaps the buffers
        void endRender();

        // This function toggles between quarter screen and full screen
        void toggleFullScreen();

        // ---- GETTERS AND SETTERS ----
        // Getter for the window
        Window &getWindow()
        {
            return *_p_mainWindow;
        }

        // Get time in seconds for rendering last frame
        double getLastFrameTime()
        {
            return _lastFrameTime_s;
        }

        // Add program to the list
        void addProgram(const char *name, const char *pathToFolder)
        {
            Shader *p_newProg = new Shader(pathToFolder);
            _progList.addBegin(p_newProg, name);
        }

        // Get program from list
        Shader *getProgram(const char *name)
        {
            return _progList.find(name);
        }

        // Get reference to camera
        Camera &getCamera()
        {
            return _camera;
        }

        // Get reference to Text camera
        Camera &getTextCamera()
        {
            return _textCamera;
        }

    private:
        // ---- FIELDS ----
        // Main window
        Window *_p_mainWindow;

        // Monitor array, current monitor and size
        GLFWmonitor **_p_monitors;
        int _currMonitor;
        int _numMonitors;

        // Screen framebuffer and texture
        GLuint _screenBuffer;
        GLuint _screenTexture;

        // Screen VBO
        GLuint _screenVAO;
        GLuint _screenVBO;

        // Cameras
        Camera _camera;
        Camera _textCamera;

        // List of programs
        GList<Shader *> _progList;

        // Identifier for the VAO
        GLuint _vaoId;

        // Time in seconds the last frame rendering lasted
        double _lastFrameTime_s;

        // Time at which rendering was last started
        double _lastStartRender_s;

        // ---- METHODS ----
        // Parse file method
        void parseFile(const char *pathToFile);

};


#endif // H_MODULES_GRAPHICS_GRAPHICS_H
