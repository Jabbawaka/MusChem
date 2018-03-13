#ifndef H_MODULES_GRAPHICS_GRAPH_H
#define H_MODULES_GRAPHICS_GRAPH_H


// Project include files
#include "GUtils.h"
#include "GLheaders.h"

// System include files
#include <vector>

class Graph
{
    public:
        // Regular constructor
        Graph(std::vector<glm::vec2> &values,
              glm::vec2 pos_pix, glm::vec2 dim_pix,
              glm::vec2 xLimits, glm::vec2 yLimits);

        // Destructor
        ~Graph();

        // Update, handle mouse logic here
        void update();

        // Render graph points joined with lines
        void render(glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f));

    private:
        // Reference to values to be rendered
        std::vector<glm::vec2> &_values;

        // Position in pixels of the bottom left corner of the graph
        glm::vec2 _pos_pix;

        // Dimensions of the graph
        glm::vec2 _dim_pix;

        // Limits of the axis
        glm::vec2 _minLimits;
        glm::vec2 _maxLimits;

        // Flag indicating whether its being controlled by the mouse
        bool _isControlledFlag;

        // Index of point being controlled
        int _pointControlled;

        // Vertex Buffer Object
        GLuint _vbo;

};

#endif // H_MODULES_GRAPHICS_GRAPH_H
