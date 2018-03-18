// Self include files
#include "Modules/Graphics/Switch.h"

// Project include files
#include "GUtils.h"
#include "GLheaders.h"
#include "GLogger/GLogger.h"

#include "Modules/Graphics/Shader.h"

#include "Systems/BasicModules.h"

// System include files


Switch::Switch(bool &value, glm::vec2 pos_pix) : _value(value)
{
    _pos_pix = pos_pix;

    glGenBuffers(1, &_vbo);
}

Switch::~Switch()
{
    glDeleteBuffers(1, &_vbo);
}

void Switch::update()
{
    // Get mouse position
    glm::vec2 mousePos_pix = input.getMousePos();

    if(input.wasLeftMouseClicked() == true &&
        mousePos_pix.x <= _pos_pix.x + 24.0f &&
        mousePos_pix.x >= _pos_pix.x &&
        mousePos_pix.y <= _pos_pix.y + 24.0f &&
        mousePos_pix.y >= _pos_pix.y)
    {
        _value = !_value;
    }
}

void Switch::render(glm::vec3 color)
{
    // ---- DRAW FRAME ----
    float frameVert[] = {
        _pos_pix.x        , _pos_pix.y        , 0.0f,
        _pos_pix.x + 24.0f, _pos_pix.y        , 0.0f,
        _pos_pix.x + 24.0f, _pos_pix.y + 24.0f, 0.0f,
        _pos_pix.x        , _pos_pix.y        , 0.0f,
        _pos_pix.x        , _pos_pix.y + 24.0f, 0.0f,
        _pos_pix.x + 24.0f, _pos_pix.y + 24.0f, 0.0f,
    };

    Shader *p_shader = graphics.getProgram("primitives");
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
        6 * 3 * sizeof(float), frameVert,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ---- CALCULATE OFF AND ON POSITIONS ----
    float yPosOff_pix;
    float yPosOn_pix;
    if(_value == true)
    {
        yPosOff_pix = _pos_pix.y + 2.0f;
        yPosOn_pix = _pos_pix.y + 12.0f;
    }
    else
    {
        yPosOff_pix = _pos_pix.y + 12.0f;
        yPosOn_pix = _pos_pix.y + 2.0f;
    }

    // ---- DRAW OFF BIT ----
    float offVert[] = {
        _pos_pix.x +  2.0f, yPosOff_pix        , 0.1f,
        _pos_pix.x +  2.0f, yPosOff_pix + 10.0f, 0.1f,
        _pos_pix.x + 22.0f, yPosOff_pix + 10.0f, 0.1f,
        _pos_pix.x +  2.0f, yPosOff_pix        , 0.1f,
        _pos_pix.x + 22.0f, yPosOff_pix        , 0.1f,
        _pos_pix.x + 22.0f, yPosOff_pix + 10.0f, 0.1f,
    };

    glUniform3f(colorId, 0.4f, 0.4f, 0.4f);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * 3 * sizeof(float), offVert,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ---- DRAW ON BIT ----
    float onVert[] = {
        _pos_pix.x +  2.0f, yPosOn_pix        , 0.1f,
        _pos_pix.x +  2.0f, yPosOn_pix + 10.0f, 0.1f,
        _pos_pix.x + 22.0f, yPosOn_pix + 10.0f, 0.1f,
        _pos_pix.x +  2.0f, yPosOn_pix        , 0.1f,
        _pos_pix.x + 22.0f, yPosOn_pix        , 0.1f,
        _pos_pix.x + 22.0f, yPosOn_pix + 10.0f, 0.1f,
    };

    glUniform3f(colorId, color.r, color.g, color.b);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * 3 * sizeof(float), onVert,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
}