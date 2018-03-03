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

    //GLOG_MSG("Point |  Original |   Graph   | Screen");
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

        //GLOG_MSG("%5d | %0.2f,%0.2f | %0.2f,%0.2f | %0.2f,%0.2f",
            //iPoint + 1, point.x, point.y, xPos_graph, yPos_graph,
            //xPos_screen, yPos_screen);

        pts_screen.push_back(glm::vec2(xPos_screen, yPos_screen));
    }

    // ---- DRAW FRAME ----
    float frameVertices[] = {
        _pos_pix.x,              _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y + _dim_pix.y, 0.0f,
        _pos_pix.x,              _pos_pix.y + _dim_pix.y, 0.0f
    };

    Shader *p_shader = graphics.getProgram("lines");
    glm::mat4 totalMat = glm::ortho(-640.0f, 640.0f, -512.0f, 512.0f); // graphics.getCamera().getMatrix();

    /*GLOG_MSG("Matrix\n"
        "%0.5f, %0.5f, %0.5f, %0.5f\n"
        "%0.5f, %0.5f, %0.5f, %0.5f\n"
        "%0.5f, %0.5f, %0.5f, %0.5f\n"
        "%0.5f, %0.5f, %0.5f, %0.5f",
        totalMat[0][0], totalMat[0][1], totalMat[0][2], totalMat[0][3],
        totalMat[1][0], totalMat[1][1], totalMat[1][2], totalMat[1][3],
        totalMat[2][0], totalMat[2][1], totalMat[2][2], totalMat[2][3],
        totalMat[3][0], totalMat[3][1], totalMat[3][2], totalMat[3][3]);*/

    p_shader->activate();

    GLuint matrixId = glGetUniformLocation(p_shader->getId(), "projection");
    GLuint colorId = glGetUniformLocation(p_shader->getId(), "provColor");

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &totalMat[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

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

#if 0
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

    if(graphics.getProgram("lines") == NULL)
    {
        GLOG_MSG("No lines shader found");
    }
    GLuint progId = graphics.getProgram("lines")->getId();
    graphics.getProgram("lines")->activate();

    GLuint matrixId = glGetUniformLocation(progId, "projection");
    GLuint colorId = glGetUniformLocation(progId, "provColor");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        3 * pts_screen.size() * sizeof(float), &lineVertices[0],
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINES, 0, pts_screen.size());

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
    if(graphics.getProgram("squares") == NULL)
    {
        GLOG_MSG("No squares shader found");
    }
    GLuint progId = graphics.getProgram("squares")->getId();
    graphics.getProgram("squares")->activate();

    glm::mat4 projMatrix = graphics.getCamera().getMatrix();

    GLuint matrixId = glGetUniformLocation(progId, "projection");
    GLuint colorId = glGetUniformLocation(progId, "color");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        3 * pts_screen.size() * sizeof(float), squareVertices,
        GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_POINTS, 0, 4 * pts_screen.size());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

    //delete lineVertices;
    //delete squareVertices;
}
