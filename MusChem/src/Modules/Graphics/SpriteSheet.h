#ifndef H_GRAPHICS_SPRITESHEET_H
#define H_GRAPHICS_SPRITESHEET_H


// Project include files
#include "GLheaders.h"

#include "Modules/Graphics/Shader.h"

// System include files
#include <vector>


// Constant definitions
#define SPRITESHEET_MAX_NUM_LAYERS (100)


// Sprite Rectangle struct:
// This structure defines a Rectangle to extract sprites from the sheet.
struct SpriteRect
{
    // Position inside the sheet: origin is at top left corner
    int xPos_pix, yPos_pix;

    // Dimensions in pixels
    int width_pix, height_pix;
};


// Sprite Sheet class:
// This class defines a collection of sprites that can be extracted using the
// Sprite Rectangle structure. It contains a vector of Sprite Rectangles as
// well separating the existing sprites.
class SpriteSheet
{
    public:
        // Constructor
        SpriteSheet();

        // Load from file
        void loadFromFile(const char *pathToFile);

        // Load error
        void loadError(glm::vec2 size_pix);

        // Render: renders the whole spritesheet in the centre of the window
        // and occupying the whole window
        void render();

        // Render: renders the selected sprite in the position given, keeping
        // dimensions
        void render(int iSprite, float xPos_pix, float yPos_pix);

        // Render error: renders a red rectangle because there is no
        // SpriteSheet loaded.
        void renderError(float xPos_pix, float yPos_pix);

    private:
        // OpenGL texture identifier
        GLuint _textureId;

        // Vertex VBO identifier
        GLuint _vertexVBO;

        // Index VBO identifier
        GLuint _indexVBO;

        // Shader used to render
        Shader *_p_shader;

        // Layer number
        int _layerNum;

        // Dimesions of the texture in pixels
        int _width_pix, _height_pix;

        // Sprites in the sheet
        std::vector<SpriteRect> _spriteRectVec;

        // Loaded spritesheet flag
        bool _loadedSpriteFlag;

};


#endif // H_GRAPHICS_SPRITESHEET_H
