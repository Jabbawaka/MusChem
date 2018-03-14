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


Graph::Graph
   (std::vector<glm::vec2> &values,
    glm::vec2 pos_pix, glm::vec2 dim_pix,
    glm::vec2 minLimits, glm::vec2 maxLimits,
    glm::vec2 spacing) : _values(values)
{
    _pos_pix = pos_pix;
    _dim_pix = dim_pix;
    _minLimits = minLimits;
    _maxLimits = maxLimits;
    _spacing = spacing;

    _isControlledFlag = false;

    glGenBuffers(1, &_vbo);
}

Graph::~Graph()
{
    glDeleteBuffers(1, &_vbo);
}

void Graph::update()
{
    // Get mouse position
    glm::vec2 mousePos_pix = input.getMousePos();
    
    if(_isControlledFlag == true)
    {
        if(input.isLeftMouseDown() == false)
        {
            _isControlledFlag = false;
            _pointControlled = -1;
        }
        else
        {
            // First and last points are special, move only y
            if(_pointControlled == 0 || _pointControlled == (int)_values.size() - 1)
            {
                _values[_pointControlled].y = _minLimits.y +
                   (mousePos_pix.y - _pos_pix.y) / _dim_pix.y *
                   (_maxLimits.y - _minLimits.y);
                if(_values[_pointControlled].y < _minLimits.y)
                {
                    _values[_pointControlled].y = _minLimits.y;
                }
                else if(_values[_pointControlled].y > _maxLimits.y)
                {
                    _values[_pointControlled].y = _maxLimits.y;
                }
            }
            else
            {
                // Get previous and next points
                glm::vec2 prevPoint = _values[_pointControlled - 1];
                glm::vec2 nextPoint = _values[_pointControlled + 1];

                _values[_pointControlled] = _minLimits +
                   (mousePos_pix - _pos_pix) / _dim_pix *
                   (_maxLimits - _minLimits);
                if(_values[_pointControlled].x < prevPoint.x)
                {
                    _values[_pointControlled].x = prevPoint.x + 0.001f;
                }
                else if(_values[_pointControlled].x > nextPoint.x)
                {
                    _values[_pointControlled].x = nextPoint.x - 0.001f;
                }

                if(_values[_pointControlled].y < _minLimits.y)
                {
                    _values[_pointControlled].y = _minLimits.y;
                }
                else if(_values[_pointControlled].y > _maxLimits.y)
                {
                    _values[_pointControlled].y = _maxLimits.y;
                }
            }
        }
    }
    else
    {
        bool anyPressed = false;
        // Not currently in control, check if in control of anything
        for(unsigned int iPoint = 0; iPoint < _values.size(); iPoint++)
        {
            // Get position of point in screen space
            glm::vec2 pointPos_pix;
            glm::vec2 pointValue = _values[iPoint];
            pointPos_pix = _pos_pix + _dim_pix *
               (pointValue - _minLimits) / (_maxLimits - _minLimits);

            if(input.wasLeftMousePressed() == true)
            {
                if(mousePos_pix.x <= pointPos_pix.x + 4.0f &&
                   mousePos_pix.x >= pointPos_pix.x - 4.0f &&
                   mousePos_pix.y <= pointPos_pix.y + 4.0f &&
                   mousePos_pix.y >= pointPos_pix.y - 4.0f)
                {
                    _isControlledFlag = true;
                    _pointControlled = iPoint;
                    anyPressed = true;
                }
            }
            else if(input.wasRightMousePressed() == true)
            {
                if(mousePos_pix.x <= pointPos_pix.x + 4.0f &&
                   mousePos_pix.x >= pointPos_pix.x - 4.0f &&
                   mousePos_pix.y <= pointPos_pix.y + 4.0f &&
                   mousePos_pix.y >= pointPos_pix.y - 4.0f)
                {
                    if(iPoint != 0 && iPoint != _values.size() - 1)
                    {
                        _values.erase(_values.begin() + iPoint);
                    }
                }
            }
        }

        if(anyPressed == false)
        {
            // Check if pressed in graph but not on point
            if(input.wasLeftMousePressed() == true)
            {
                for(unsigned int iPoint = 0; iPoint < _values.size() - 1; iPoint++)
                {
                    glm::vec2 prevPointPos_pix = _pos_pix + _dim_pix *
                       (_values[iPoint] - _minLimits) / (_maxLimits - _minLimits);
                    glm::vec2 nextPointPos_pix = _pos_pix + _dim_pix *
                       (_values[iPoint + 1] - _minLimits) / (_maxLimits - _minLimits);

                    if(mousePos_pix.x > prevPointPos_pix.x &&
                        mousePos_pix.x < nextPointPos_pix.x &&
                        mousePos_pix.y > _pos_pix.y &&
                        mousePos_pix.y < _pos_pix.y + _dim_pix.y)
                    {
                        glm::vec2 mouseValue =
                            _minLimits +
                            (mousePos_pix - _pos_pix) / _dim_pix * (_maxLimits - _minLimits);
                        _values.insert(_values.begin() + iPoint + 1, mouseValue);
                        _pointControlled = iPoint + 1;
                        _isControlledFlag = true;
                        break;
                    }
                }
            }
        }
    }
}

void Graph::render
   (glm::vec3 color)
{
    // ---- GET POINTS IN SCREEN FRAME ----
    std::vector<glm::vec2> pts_screen;

    for(unsigned int iPoint = 0; iPoint < _values.size(); iPoint++)
    {
        // Current point
        glm::vec2 point = _values[iPoint];
        
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

    // ---- DRAW FRAME ----
    float frameVertices[] = {
        _pos_pix.x,              _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y,              0.0f,
        _pos_pix.x + _dim_pix.x, _pos_pix.y + _dim_pix.y, 0.0f,
        _pos_pix.x,              _pos_pix.y + _dim_pix.y, 0.0f
    };

    Shader *p_shader = graphics.getProgram("primitives");
    glm::mat4 projMatrix = graphics.getCamera().getMatrix();

    p_shader->activate();

    GLuint matrixId = glGetUniformLocation(p_shader->getId(), "projection");
    GLuint colorId = glGetUniformLocation(p_shader->getId(), "provColor");

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, 0.4f, 0.4f, 0.4f);

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

    // ---- DRAW SPACING ----
    glm::vec2 lines = (_maxLimits - _minLimits) / _spacing;
    int horLines = ceil(lines.y - 1);
    int verLines = ceil(lines.x - 1);
    float *spaceVert = new float[3 * 2 * horLines * verLines];

    for(int iHor = 0; iHor < horLines; iHor++)
    {
        spaceVert[6 * iHor + 0] = _pos_pix.x;
        spaceVert[6 * iHor + 1] = _pos_pix.y + _dim_pix.y *
           ((iHor + 1) * _spacing.y + _minLimits.y) /
            (_maxLimits.y - _minLimits.y);
        spaceVert[6 * iHor + 2] = 0.1f;

        spaceVert[6 * iHor + 3] = _pos_pix.x + _dim_pix.x;
        spaceVert[6 * iHor + 4] = _pos_pix.y + _dim_pix.y *
           ((iHor + 1) * _spacing.y + _minLimits.y) /
            (_maxLimits.y - _minLimits.y);
        spaceVert[6 * iHor + 5] = 0.1f;
    }
    for(int iVer = 0; iVer < verLines; iVer++)
    {
        spaceVert[6 * horLines + 6 * iVer + 0] =
            _pos_pix.x + _dim_pix.x *
            ((iVer + 1) * _spacing.x + _minLimits.x) /
            (_maxLimits.x - _minLimits.x);
        spaceVert[6 * horLines + 6 * iVer + 1] = _pos_pix.y;
        spaceVert[6 * horLines + 6 * iVer + 2] = 0.1f;

        spaceVert[6 * horLines + 6 * iVer + 3] =
            _pos_pix.x + _dim_pix.x *
            ((iVer + 1) * _spacing.x + _minLimits.x) /
            (_maxLimits.x - _minLimits.x);
        spaceVert[6 * horLines + 6 * iVer + 4] = _pos_pix.y + _dim_pix.y;
        spaceVert[6 * horLines + 6 * iVer + 5] = 0.1f;
    }

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, 0.2f, 0.2f, 0.2f);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * horLines * verLines * sizeof(float), &spaceVert[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINES, 0, 2 * horLines * verLines);

    glDisableVertexAttribArray(0);

    // ---- DRAW LINES ----
    // Create vertices
    float *lineVertices = new float[3 * pts_screen.size()];
    for(unsigned int iVert = 0; iVert < pts_screen.size(); iVert++)
    {
        lineVertices[3 * iVert + 0] = pts_screen[iVert].x;
        lineVertices[3 * iVert + 1] = pts_screen[iVert].y;
        lineVertices[3 * iVert + 2] = 1.0f;
    }

    // All the OpenGL stuff
    colorId = glGetUniformLocation(p_shader->getId(), "provColor");
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
        squareVert[6 * 3 * iVert + 3 * 0 + 2] = 1.0f;

        squareVert[6 * 3 * iVert + 3 * 1 + 0] = pts_screen[iVert].x - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 1 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 1 + 2] = 1.0f;

        squareVert[6 * 3 * iVert + 3 * 2 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 2 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 2 + 2] = 1.0f;

        squareVert[6 * 3 * iVert + 3 * 3 + 0] = pts_screen[iVert].x - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 3 + 1] = pts_screen[iVert].y - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 3 + 2] = 1.0f;

        squareVert[6 * 3 * iVert + 3 * 4 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 4 + 1] = pts_screen[iVert].y - 2.0f;
        squareVert[6 * 3 * iVert + 3 * 4 + 2] = 1.0f;

        squareVert[6 * 3 * iVert + 3 * 5 + 0] = pts_screen[iVert].x + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 5 + 1] = pts_screen[iVert].y + 2.0f;
        squareVert[6 * 3 * iVert + 3 * 5 + 2] = 1.0f;
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
