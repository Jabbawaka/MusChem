// Self include files
#include "Modules/GUI/Slider.h"

// Project include files
#include "GUtils.h"
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "Modules/Graphics/Shader.h"

#include "Systems/BasicModules.h"

// System include files


Slider::Slider(float *p_value)
{
    _p_value = p_value;

    _pos_pix = glm::vec2(0.0f, 0.0f);
    _length_pix = 0.0f;
    _limits = glm::vec2(0.0f, 0.0f);

    _isControlledFlag = false;
}

Slider::Slider
   (float *p_value, bool resetFlag,
    glm::vec2 pos_pix, float length_pix, glm::vec2 limits)
{
    _p_value = p_value;

    _pos_pix = pos_pix;
    _length_pix = length_pix;
    _limits = limits;

    _isControlledFlag = false;
    _resetOnLeaveFlag = resetFlag;

    glGenBuffers(1, &_vbo);
}

Slider::~Slider()
{
    _p_value = NULL;

    glDeleteBuffers(1, &_vbo);
}

void Slider::update()
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
            *_p_value = _limits.x +
                (mousePos_pix.y - _pos_pix.y)
                / _length_pix * (_limits.y - _limits.x);
            if(*_p_value < _limits.x)
            {
                *_p_value = _limits.x;
            }
            else if(*_p_value > _limits.y)
            {
                *_p_value = _limits.y;
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
        else
        {
            if(_resetOnLeaveFlag == true)
            {
                *_p_value = (_limits.x + _limits.y) / 2.0f;
            }
        }
    }
}

void Slider::render(glm::vec3 color)
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

    // ---- DRAW SQUARE ----
    // Get position of center of square in screen space
    glm::vec2 pos_pix_screen;
    pos_pix_screen.x = _pos_pix.x;
    pos_pix_screen.y = _pos_pix.y +
        _length_pix * (*_p_value - _limits.x) / (_limits.y - _limits.x);

    float squareVert[] = {
        pos_pix_screen.x - 10.0f, pos_pix_screen.y - 2.0f, 1.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y - 2.0f, 1.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y + 2.0f, 1.0f,
        pos_pix_screen.x - 10.0f, pos_pix_screen.y - 2.0f, 1.0f,
        pos_pix_screen.x - 10.0f, pos_pix_screen.y + 2.0f, 1.0f,
        pos_pix_screen.x + 10.0f, pos_pix_screen.y + 2.0f, 1.0f
    };
 
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * 3 * sizeof(float), &squareVert[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
}
