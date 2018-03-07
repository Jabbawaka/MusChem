// Self include files
#include "Modules/Graphics/Graph.h"

// Project include files
#include "GUtils.h"
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "Modules/Graphics/Shader.h"

#include "Systems/BasicModules.h"

// System include files
#include <vector>


Graph::Graph()
{
    _pos_pix = glm::vec2(0.0f);
    _dim_pix = glm::vec2(0.0f);

    glGenBuffers(1, &_vbo);
}

Graph::Graph
   (glm::vec2 pos_pix, glm::vec2 dim_pix,
    glm::vec2 minLimits, glm::vec2 maxLimits)
{
    _pos_pix = pos_pix;
    _dim_pix = dim_pix;
    _minLimits = minLimits;
    _maxLimits = maxLimits;

    glGenBuffers(1, &_vbo);
}

Graph::~Graph()
{
    glDeleteBuffers(1, &_vbo);
}

void Graph::render
   (std::vector<glm::vec2> points,
    glm::vec3 color)
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
            (point.y - _minLimits.y) / (_maxLimits.y - _minLimits.y);

        // Get point coordinates in the screen frame
        float xPos_screen = _pos_pix.x + xPos_graph * _dim_pix.x;
        float yPos_screen = _pos_pix.y + yPos_graph * _dim_pix.y;

        pts_screen.push_back(glm::vec2(xPos_screen, yPos_screen));
    }
    glm::vec2 lastPoint = pts_screen[pts_screen.size() - 1];
    lastPoint.x = _pos_pix.x + _dim_pix.x;
    pts_screen.push_back(lastPoint);

    // ---- DRAW FRAME ----
    float frameVertices[] = {
        _pos_pix.x,              _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y + _dim_pix.y, 0.0f,
        _pos_pix.x,              _pos_pix.y + _dim_pix.y, 0.0f
    };

    Shader *p_shader = graphics.getProgram("lines");
    glm::mat4 projMatrix = graphics.getCamera().getMatrix();

    p_shader->activate();

    GLuint matrixId = glGetUniformLocation(p_shader->getId(), "projection");
    GLuint colorId = glGetUniformLocation(p_shader->getId(), "provColor");

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, 0.2f, 0.2f, 0.2f);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        4 * 3 * sizeof(float), &frameVertices[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glDisableVertexAttribArray(0);

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
    projMatrix = graphics.getCamera().getMatrix();

    GLuint progId = graphics.getProgram("lines")->getId();
    graphics.getProgram("lines")->activate();

    matrixId = glGetUniformLocation(progId, "projection");
    colorId = glGetUniformLocation(progId, "provColor");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        3 * pts_screen.size() * sizeof(float), &lineVertices[0],
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINE_STRIP, 0, pts_screen.size());

    glDisableVertexAttribArray(0);

    delete lineVertices;

    // ---- DRAW SQUARES ----
    // Create vertices
    float *squareVert = new float[6 * 3 * pts_screen.size()];
    for(unsigned int iVert = 0; iVert < pts_screen.size(); iVert++)
    {
        squareVert[6 * 3 * iVert + 3 * 0 + 0] = pts_screen[iVert].x - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 0 + 1] = pts_screen[iVert].y - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 0 + 2] = 0.0f;

        squareVert[6 * 3 * iVert + 3 * 1 + 0] = pts_screen[iVert].x - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 1 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 1 + 2] = 0.0f;

        squareVert[6 * 3 * iVert + 3 * 2 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 2 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 2 + 2] = 0.0f;

        squareVert[6 * 3 * iVert + 3 * 3 + 0] = pts_screen[iVert].x - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 3 + 1] = pts_screen[iVert].y - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 3 + 2] = 0.0f;

        squareVert[6 * 3 * iVert + 3 * 4 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 4 + 1] = pts_screen[iVert].y - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 4 + 2] = 0.0f;

        squareVert[6 * 3 * iVert + 3 * 5 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 5 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 5 + 2] = 0.0f;
    }

    glBufferData
       (GL_ARRAY_BUFFER,
        18 * pts_screen.size() * sizeof(float), &squareVert[0],
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 18 * pts_screen.size());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete squareVert;
}
