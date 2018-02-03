// Self include files
#include "Modules/Graphics/SpriteSheet.h"

// Project include files
#include "GLheaders.h"
#include "GLogger/GLogger.h"
#include "GParser/GParser.h"

#include "Systems/BasicModules.h"

#include "SOIL.h"

// System include files


SpriteSheet::SpriteSheet()
{
    // Assign invalid value to texture
    _textureId = 0;

    // Get VBOs
    glGenBuffers(1, &_vertexVBO);
    glGenBuffers(1, &_indexVBO);

    _loadedSpriteFlag = false;
}

void SpriteSheet::loadFromFile(const char *pathToFile)
{
    // Parse file
    GParser parser(pathToFile);
    GParser_Line line;

    while(parser.readNextData(line) == true)
    {
        if(line.key == "file")
        {
            // Load image from file
            unsigned char *p_pixels = SOIL_load_image
               (line.value.c_str(), &_width_pix, &_height_pix, NULL, SOIL_LOAD_RGBA);
            if(p_pixels == NULL)
            {
                GLOG_ERR("Could not load SpriteSheet from %s", line.value.c_str());
                throw "Could not load image file";
            }

            // Tell OpenGL to generate the texture
            glGenTextures(1, &_textureId);

            // Bind and buffer the data
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _textureId);

            glTexImage2D
               (GL_TEXTURE_2D, 0, GL_RGBA,
                _width_pix, _height_pix,
                0, GL_RGBA, GL_UNSIGNED_BYTE, p_pixels);

            // Set filters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            // Free image since we don't need it anymore
            SOIL_free_image_data(p_pixels);
        }
        else if(line.key == "shader")
        {
            _p_shader = graphics.getProgram(line.value.c_str());
        }
        else if(line.key == "sprite")
        {
            SpriteRect rect;

            rect.xPos_pix = atoi(strtok(&line.value[0], ","));
            rect.yPos_pix = atoi(strtok(NULL, ","));
            rect.width_pix = atoi(strtok(NULL, ","));
            rect.height_pix = atoi(strtok(NULL, "\n"));

            _spriteRectVec.push_back(rect);
        }
        else if(line.key == "layer")
        {
            _layerNum = atoi(line.value.c_str());
        }
    }

    _loadedSpriteFlag = true;
}

void SpriteSheet::loadError(glm::vec2 size_pix)
{
    _p_shader = graphics.getProgram("error");

    _width_pix = (int) size_pix.x;
    _height_pix = (int) size_pix.y;

    _layerNum = 0;
}

void SpriteSheet::render()
{
    glm::mat4 idMatrix = glm::mat4(1.0f);

    _p_shader->activate();

    GLuint matrixId = glGetUniformLocation(_p_shader->getId(), "projection");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &idMatrix[0][0]);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  0.0,  1.0f,
        -0.5f,  0.5f, 0.0f,  0.0,  0.0f,
         0.5f, -0.5f, 0.0f,  1.0,  1.0f,
         0.5f,  0.5f, 0.0f,  1.0,  0.0f,
    };

    GLuint indices[] = {
        0, 1, 3,
        0, 3, 2
    };

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO);
    glBufferData
       (GL_ARRAY_BUFFER,
        4 * 5 * sizeof(float), &vertices[0],
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
    glBufferData
       (GL_ELEMENT_ARRAY_BUFFER,
        6 * sizeof(GLuint), &indices[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer
       (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid *) 12);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void SpriteSheet::render(int iSprite, float xPos_pix, float yPos_pix)
{
    // Dirty hack to intercept rendering when sprite has not been loaded
    if(_loadedSpriteFlag == false)
    {
        renderError(xPos_pix, yPos_pix);
        return;
    }

    // Get camera matrix
    glm::mat4 camMatrix = graphics.getCamera().getMatrix();

    // Scaling matrix to get sprite size
    glm::mat4 sizeSpriteMatrix = glm::scale
      (glm::vec3((float) _spriteRectVec[iSprite].width_pix,
      (float) _spriteRectVec[iSprite].height_pix,
       -1.0f));

    // Translate to position in which to render in global frame
    glm::mat4 posSpriteMatrix = glm::translate
       (glm::mat4(), glm::vec3((float) xPos_pix, (float) yPos_pix, 0.0f));

    // Generate final matrix and send to shader
    glm::mat4 totalMatrix = camMatrix * posSpriteMatrix * sizeSpriteMatrix;

    _p_shader->activate();

    GLuint matrixId = glGetUniformLocation(_p_shader->getId(), "projection");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &totalMatrix[0][0]);

    // Locate UV coordinates
    float uMin =
        (float) _spriteRectVec[iSprite].xPos_pix / (float) _width_pix;
    float uMax =
       (float) (_spriteRectVec[iSprite].xPos_pix +
        _spriteRectVec[iSprite].width_pix) /
        (float) _width_pix;
    float vMin =
        (float) _spriteRectVec[iSprite].yPos_pix / (float) _height_pix;
    float vMax =
       (float) (_spriteRectVec[iSprite].yPos_pix +
        _spriteRectVec[iSprite].height_pix) /
        (float) _height_pix;

    float vertices[] = {
       -0.5f, -0.5f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        uMin,  vMax,

       -0.5f,  0.5f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        uMin,  vMin,

        0.5f, -0.5f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        uMax,  vMax,

        0.5f,  0.5f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        uMax,  vMin
    };

    GLuint indices[] = {
        0, 1, 3,
        0, 3, 2
    };

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO);
    glBufferData
       (GL_ARRAY_BUFFER,
        4 * 5 * sizeof(float), &vertices[0],
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
    glBufferData
       (GL_ELEMENT_ARRAY_BUFFER,
        6 * sizeof(GLuint), &indices[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer
       (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid *) 12);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void SpriteSheet::renderError(float xPos_pix, float yPos_pix)
{
    // Get camera matrix
    glm::mat4 camMatrix = graphics.getCamera().getMatrix();

    // Scaling matrix to get sprite size
    glm::mat4 sizeSpriteMatrix = glm::scale
      (glm::vec3((float) _width_pix, (float) _height_pix, -1.0f));

    // Translate to position in which to render in global frame
    glm::mat4 posSpriteMatrix = glm::translate
       (glm::mat4(), glm::vec3((float) xPos_pix, (float) yPos_pix, 0.0f));

    // Generate final matrix and send to shader
    glm::mat4 totalMatrix = camMatrix * posSpriteMatrix * sizeSpriteMatrix;

    _p_shader->activate();

    GLuint matrixId = glGetUniformLocation(_p_shader->getId(), "projection");
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, &totalMatrix[0][0]);

    float vertices[] = {
       -1.0f, -1.0f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
       -1.0f,  1.0f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        1.0f, -1.0f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
        1.0f,  1.0f, (float) _layerNum / SPRITESHEET_MAX_NUM_LAYERS,
    };

    GLuint indices[] = {
        0, 1, 3,
        0, 3, 2
    };

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO);
    glBufferData
       (GL_ARRAY_BUFFER,
        4 * 3 * sizeof(float), &vertices[0],
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
    glBufferData
       (GL_ELEMENT_ARRAY_BUFFER,
        6 * sizeof(GLuint), &indices[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
}
