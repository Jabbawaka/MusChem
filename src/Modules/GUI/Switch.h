#ifndef H_MODULES_GRAPHICS_SWITCH_H
#define H_MODULES_GRAPHICS_SWITCH_H


// Project include files
#include "GUtils.h"
#include "GLheaders.h"

// System include files


class Switch
{
    public:
        // Regular constructor
        Switch(bool &value, glm::vec2 pos_pix);

        // Destructor
        ~Switch();

        // Update
        void update();

        // Render switch
        void render(glm::vec3 color = glm::vec3(0.7f));

    private:
        // Pointer to the value controlled by the switch
        bool &_value;

        // Position in pixels of the bottom left corner of the switch
        glm::vec2 _pos_pix;

        // Vertex buffer object
        GLuint _vbo;

};

#endif // H_MODULES_GRAPHICS_SWITCH_H