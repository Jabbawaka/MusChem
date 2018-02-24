#ifndef H_MODULES_GRAPHICS_GRAPH_H
#define H_MODULES_GRAPHICS_GRAPH_H


// Project include files
#include "Gutils.h"
#include "GLHeaders.h"

// System include files
#include <vector>

class Graph
{
    public:
        // Empty constructor
        Graph();

        // Regular constructor
        Graph(glm::vec2 pos_pix, glm::vec2 dim_pix, glm::vec2 xLimits, glm::vec2 yLimits);

        // Render graph points joined with lines
        void render(std::vector<glm::vec2> points, glm::vec3 color);

    private:
        // Position in pixels of the bottom left corner of the graph
        glm::vec2 _pos_pix;

        // Dimensions of the graph
        glm::vec2 _dim_pix;

        // Limits of the axis
        glm::vec2 _minLimits;
        glm::vec2 _maxLimits;

        // Vertex Buffer Objects
        GLuint _vbo;

};

#endif // H_MODULES_GRAPHICS_GRAPH_H