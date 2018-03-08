// Self include files
#include "Modules/Graphics/DiscSlider.h"

// Project include files
#include "GUtils.h"
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "Modules/Graphics/Shader.h"

#include "Systems/BasicModules.h"

// System include files
#include <math.h>


DiscSlider::DiscSlider(float *p_value)
{
    _p_value = p_value;

    _pos_pix = glm::vec2(0.0f, 0.0f);
    _length_pix = 0.0f;
    _limits = glm::vec2(0.0f, 0.0f);
    _spacing = 0.0f;

    _isControlledFlag = false;
}

DiscSlider::DiscSlider
   (float *p_value,
    glm::vec2 pos_pix, float length_pix,
    glm::vec2 limits, float spacing)
{
    _p_value = p_value;

    _pos_pix = pos_pix;
    _length_pix = length_pix;
    _limits = limits;
    _spacing = spacing;

    // Correct limits
    int nLevels = lround((_limits.y - _limits.x) / _spacing) + 1;
    _limits.y = _limits.x + _spacing * (nLevels - 1);

    _isControlledFlag = false;

    glGenBuffers(1, &_vbo);
}

DiscSlider::~DiscSlider()
{
    _p_value = NULL;

    glDeleteBuffers(1, &_vbo);
}

void DiscSlider::update()
{
    // Get mouse position
    glm::vec2 mousePos_pix = input.getMousePos();

    // Get slider current position
    glm::vec2 sliderPos_pix;
    sliderPos_pix.x = _pos_pix.x;
    sliderPos_pix.y = _pos_pix.y +
        _length_pix * (*_p_value - _limits.x) / (_limits.y - _limits.x);

    if(_isControlledFlag == true)
    {
        if(input.isLeftMouseDown() == false)
        {
            _isControlledFlag = false;
        }
        else
        {
            int nLevels = (_limits.y - _limits.x) / _spacing + 1;

            for(int iLev = 0; iLev < nLevels; iLev++)
            {
                float levPos_pix =
                    _pos_pix.y +
                    _length_pix * (float) iLev / ((float) nLevels - 1);
                if(fabs(mousePos_pix.y - levPos_pix) < 5.0f)
                {
                    *_p_value = _limits.x + iLev * _spacing;
                }
            }
        }
    }
    else
    {
        // Not currently in control of the mouse, check if should be in control
        if(input.wasLeftMousePressed() == true &&
            mousePos_pix.x <= sliderPos_pix.x + 12.0f &&
            mousePos_pix.x >= sliderPos_pix.x - 12.0f &&
            mousePos_pix.y <= sliderPos_pix.y + 4.0f &&
            mousePos_pix.y >= sliderPos_pix.y - 4.0f)
        {
            _isControlledFlag = true;
        }
    }
}

void DiscSlider::render(glm::vec3 color)
{
    // ---- DRAW BACK LINE ----
    float lineVert[] = {
        _pos_pix.x, _pos_pix.y, 0.0f,
        _pos_pix.x, _pos_pix.y + _length_pix, 0.0f
    };

    Shader *p_shader = graphics.getProgram("primitives");
    glm::mat4 projMatrix = graphics.getCamera().getMatrix();

    p_shader->activate();

    GLuint matrixId = glGetUniformLocation(p_shader->getId(), "projection");
    GLuint colorId = glGetUniformLocation(p_shader->getId(), "provColor");

    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform3f(colorId, color.r, color.g, color.b);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        2 * 3 * sizeof(float), &lineVert[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableVertexAttribArray(0);

    // ---- DRAW LEVELS ----
    int numLevels = (_limits.y - _limits.x) / _spacing + 1;
    float *levelVert = new float[2 * 3 * numLevels];
    for(int iLev = 0; iLev < numLevels; iLev++)
    {
        // Left side
        levelVert[2 * 3 * iLev + 0] = _pos_pix.x - 10.0f;
        levelVert[2 * 3 * iLev + 1] =
            _pos_pix.y + _length_pix * (float) iLev / ((float) numLevels - 1);
        levelVert[2 * 3 * iLev + 2] = 1.0f;

        // Right side
        levelVert[2 * 3 * iLev + 3] = _pos_pix.x + 10.0f;
        levelVert[2 * 3 * iLev + 4] =
            _pos_pix.y + _length_pix * (float) iLev / ((float) numLevels - 1);
        levelVert[2 * 3 * iLev + 5] = 1.0f;
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        2 * 3 * numLevels * sizeof(float), &levelVert[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_LINES, 0, 2 * numLevels);

    glDisableVertexAttribArray(0);

    delete levelVert;

    // ---- DRAW SQUARE ----
    // Get position of center of square in screen space
    glm::vec2 pos_pix_screen;
    pos_pix_screen.x = _pos_pix.x;
    pos_pix_screen.y = _pos_pix.y +
        _length_pix * (*_p_value - _limits.x) / (_limits.y - _limits.x);

    float squareVert[] = {
        pos_pix_screen.x - 10.0f, pos_pix_screen.y - 2.0f, 0.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y - 2.0f, 0.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y + 2.0f, 0.0f,
        pos_pix_screen.x - 10.0f, pos_pix_screen.y - 2.0f, 0.0f,
        pos_pix_screen.x - 10.0f, pos_pix_screen.y + 2.0f, 0.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y + 2.0f, 0.0f
    };
 
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * 3 * sizeof(float), &squareVert[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 18);

    glDisableVertexAttribArray(0);
}
