#ifndef H_MODULES_GRAPHICS_CAMERA_H
#define H_MODULES_GRAPHICS_CAMERA_H


// Project includes
#include "GLheaders.h"
#include "Modules/Graphics/Window.h"

// System includes


// Constant definitions
#define CAMERA_SCREEN_WIDTH (427)
#define CAMERA_SCREEN_HEIGHT (240)

// Camera class:
// This class represents a camera that holds the appropriate transformations to
// perform to the objects in order for it to project properly.
class Camera
{
    public:
        // Constructor
        Camera();

        // Set size of the camera
        void setSize(Window &window);

        // Update position
        void update(glm::vec2 pos_pix);

        // Zoom out
        void zoomOut();

        // Zoom in
        void zoomIn();

        // Get position of the camera
        glm::vec2 getPos()
        {
            return _pos_pix;
        }

        // Get size of the camera
        glm::vec2 getSize()
        {
            return _size_pix;
        }

        // Get the VP part of MVP projection
        glm::mat4 getMatrix()
        {
            glm::mat4 posMatrix = glm::translate
               (glm::mat4(),
                glm::vec3(-_pos_pix, 0.0f));
            return _projViewMatrix * posMatrix;
        }

    private:
        // Projection matrix
        glm::mat4 _projViewMatrix;

        // Size of the camera
        glm::vec2 _size_pix;

        // Position of the camera in pixels
        glm::vec2 _pos_pix;

};


#endif // H_MODULES_GRAPHICS_CAMERA_H
