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
        // Empty constructor
        Graph();

        // Regular constructor
        Graph(glm::vec2 pos_pix, glm::vec2 dim_pix,
              glm::vec2 xLimits, glm::vec2 yLimits);

        // Destructor
        ~Graph();

        // Render graph points joined with lines
        void render(std::vector<glm::vec2> points, glm::vec3 color = glm::vec3(0.0f, 0.3f, 1.0f));

        // ---- GETTERS AND SETTERS ----
        void setPos(glm::vec2 pos_pix)
        {
            _pos_pix = pos_pix;
        }

        void setSize(glm::vec2 dim_pix)
        {
            _dim_pix = dim_pix;
        }

        void setLimits(glm::vec2 minLimits, glm::vec2 maxLimits)
        {
            _minLimits = minLimits;
            _maxLimits = maxLimits;
        }

        glm::vec2 getPos()
        {
            return _pos_pix;
        }

        glm::vec2 getSize()
        {
            return _dim_pix;
        }

    private:
        // Position in pixels of the bottom left corner of the graph
        glm::vec2 _pos_pix;

        // Dimensions of the graph
        glm::vec2 _dim_pix;

        // Limits of the axis
        glm::vec2 _minLimits;
        glm::vec2 _maxLimits;

        // Vertex Buffer Object
        GLuint _vbo;

};

#endif // H_MODULES_GRAPHICS_GRAPH_H
