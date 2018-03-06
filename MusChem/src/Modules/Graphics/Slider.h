#ifndef H_MODULES_GRAPHICS_SLIDER_H
#define H_MODULES_GRAPHICS_SLIDER_H


// Project include files
#include "GUtils.h"
#include "GLheaders.h"

// System include files

class Slider
{
    public:
        // Empty constructor
        Slider(float *p_value);

        // Regular constructor
        Slider
           (float *p_value,
            glm::vec2 pos_pix, float length_pix, glm::vec2 limits);

        // Destructor
        ~Slider();

        // Update
        void update();

        // Render slider
        void render(glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f));

    private:
        // Pointer to the floating point value controlled by the slider
        float *_p_value;

        // Position in pixels of the bottom/left side of the slider
        glm::vec2 _pos_pix;

        // Length of the slider
        float _length_pix;

        // Limits of the slider
        glm::vec2 _limits;

        // Flag indicating whether the slider is being controlled
        bool _isControlledFlag;

        // Vertex Buffer Object
        GLuint _vbo;

};

#endif // H_MODULES_GRAPHICS_SLIDER_H