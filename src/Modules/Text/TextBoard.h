#ifndef MODULES_TEXT_TEXTBOARD_H
#define MODULES_TEXT_TEXTBOARD_H


// Project include files
#include "GUtils.h"

#include "Modules/Text/Font.h"

// System include files
#include <string>


// TextBoard class:
// This class defines a board onto which to render the text. They are always
// facing the camera.
class TextBoard
{
    public:
        // Constructor
        TextBoard();

        // Render text on board in game coordinates
        void renderTextGame
           (std::string text, glm::vec3 color = glm::vec3(1.0f));

        // Render text on board in screen coordinates
        void renderTextScreen
           (std::string text, glm::vec3 color = glm::vec3(1.0f));

        // ---- GETTERS AND SETTERS ----
        // Set font
        void setFont(std::string fontName);

        // Set position
        void setPos(glm::vec2 pos_pix_glob)
        {
            _pos_pix_glob = pos_pix_glob;
        }

        // Set width
        void setWidth(float width_pix)
        {
            _size_pix.x = width_pix;
        }

        // Set margins
        void setMargins(glm::vec2 margin_pix)
        {
            _margin_pix = margin_pix;
        }

        // Get width
        float getWidth()
        {
            return _size_pix.x;
        }

    private:
        // Position of the board in pixels in the global frame
        glm::vec2 _pos_pix_glob;

        // Dimensions of the board in pixels
        glm::vec2 _size_pix;

        // Margins of text with respect to box edge
        glm::vec2 _margin_pix;

        // Font to use in the TextBoard
        Font *_p_font;

        // Texture ID
        GLuint _textureId;

        // VBO indices
        GLuint _letterVBO;
        GLuint _boxVBO;

        // ---- METHODS ----
        // Render a single character
        void renderChar
           (char character, glm::vec2 &renderPos_pix);

        // Check if the word finishes after the end of the board
        bool doesWordOverflow
           (std::string  text,
            int          iChar,
            float        xPos_pix,
            float        xPosLimit_pix);

        // Find out position and size
        void findRenderPosAndSize
           (std::string text, glm::vec2 &renderPos_pix, glm::vec2 &size_pix);

};

#endif // MODULES_TEXT_TEXTBOARD_H
