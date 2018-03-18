#ifndef H_MODULES_GRAPHICS_DISCSLIDER_H
#define H_MODULES_GRAPHICS_DISCSLIDER_H


// Project include files
#include "GUtils.h"
#include "GLheaders.h"

// System include files


class DiscSlider
{
    public:
        // Empty constructor
        DiscSlider(float *p_value);

        // Regular constructor
        DiscSlider(float *p_value,
               glm::vec2 pos_pix, float length_pix,
               glm::vec2 limits, float spacing);

        // Destructor
        ~DiscSlider();

        // Update: react to mouse to move stuff
        void update();

        // Render
        void render(glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f));

    private:
        // Pointer to the integer value controlled by the slider
        float *_p_value;

        // Position in pixels of the bottom/left side of the slider
        glm::vec2 _pos_pix;

        // Length of the slider
        float _length_pix;

        // Limits of the slider
        glm::vec2 _limits;

        // Spacing of the slider
        float _spacing;

        // Flag indicating whether the slider is being controlled
        bool _isControlledFlag;

        // Vertex Buffer Object
        GLuint _vbo;

};


#endif // H_MODULES_GRAPHICS_DISCSLIDER_H
