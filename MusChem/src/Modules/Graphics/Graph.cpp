// Self include files
#include "Modules/Graphics/Graph.h"

// Project include files
#include "GUtils.h"
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "Systems/BasicModules.h"

// System include files
#include <vector>


Graph::Graph()
{
    _pos_pix = glm::vec2(0.0f);
    _dim_pix = glm::vec2(0.0f);
}

Graph::Graph
   (glm::vec2 pos_pix, glm::vec2 dim_pix,
    glm::vec2 minLimits, glm::vec2 maxLimits)
{
    _pos_pix = pos_pix;
    _dim_pix = dim_pix;
    _minLimits = minLimits;
    _maxLimits = maxLimits;
}

void Graph::render
   (std::vector<glm::vec2> points,
    glm::vec3 color = glm::vec3(0.0f, 0.3f, 1.0f))
{
    // ---- GET POINTS IN SCREEN FRAME ----
    std::vector<glm::vec2> pts_screen;

    for(unsigned int iPoint = 0; iPoint < points.size(); iPoint++)
    {
        // Current point
        glm::vec2 point = points[iPoint];
        
        // Get points coordinates in graph's frame
        float xPos_graph =
            (point.x - _minLimits.x) / (_maxLimits.x - _minLimits.x);
        float yPos_graph =
            (point.y - _minLimits.y) / (_maxLimits.x - _minLimits.x);

        // Get point coordinates in the screen frame
        float xPos_screen = _pos_pix.x + xPos_graph * _dim_pix.x;
        float yPos_screen = _pos_pix.y + yPos_graph * _dim_pix.y;

        pts_screen.push_back(glm::vec2(xPos_screen, yPos_screen));
    }

    // ---- DRAW LINES ----
    // Create vertices
    float *lineVertices = new float[3 * pts_screen.size()];
    for(unsigned int iVert = 0; iVert < pts_screen.size(); iVert++)
    {
        lineVertices[3 * iVert + 0] = pts_screen[iVert].x;
        lineVertices[3 * iVert + 1] = pts_screen[iVert].y;
        lineVertices[3 * iVert + 2] = 0.0f;
    }

    // All the OpenGL stuff
    glm::mat4 projMatrix = graphics.getCamera().getMatrix();

    GLuint progId = graphics.getProgram("lines")->getId();
    graphics.getProgram("lines")->activate();

    GLuint matrixId = glGetUniformLocation(progId, "projection");
    GLuint colorId = glGetUniformLocation(progId, "color");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        3 * pts_screen.size() * sizeof(float), lineVertices,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINE, 0, pts_screen.size());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ---- DRAW SQUARES ----
    // Create vertices for squares
    float *squareVertices = new float[3 * 4 * pts_screen.size()];
    for(unsigned int iVert = 0; iVert < pts_screen.size(); iVert++)
    {
        // Bottom left
        squareVertices[3 * iVert + 0] = pts_screen[iVert].x - 10.0f;
        squareVertices[3 * iVert + 1] = pts_screen[iVert].y - 10.0f;
        squareVertices[3 * iVert + 2] = 0.0f;

        // Bottom right
        squareVertices[3 * iVert + 3] = pts_screen[iVert].x + 10.0f;
        squareVertices[3 * iVert + 4] = pts_screen[iVert].y - 10.0f;
        squareVertices[3 * iVert + 5] = 0.0f;

        // Top right
        squareVertices[3 * iVert + 6] = pts_screen[iVert].x + 10.0f;
        squareVertices[3 * iVert + 7] = pts_screen[iVert].y + 10.0f;
        squareVertices[3 * iVert + 8] = 0.0f;

        // Top left
        squareVertices[3 * iVert + 9] = pts_screen[iVert].x - 10.0f;
        squareVertices[3 * iVert + 10] = pts_screen[iVert].y + 10.0f;
        squareVertices[3 * iVert + 11] = 0.0f;
    }

    // All the OpenGL stuff
    progId = graphics.getProgram("squares")->getId();
    graphics.getProgram("squares")->activate();

    matrixId = glGetUniformLocation(progId, "projection");
    colorId = glGetUniformLocation(progId, "color");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        3 * pts_screen.size() * sizeof(float), squareVertices,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_QUADS, 0, pts_screen.size());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete lineVertices;
    delete squareVertices;
}
