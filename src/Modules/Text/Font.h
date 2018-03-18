#ifndef H_MODULES_TEXT_FONT_H
#define H_MODULES_TEXT_FONT_H


// Project includes
#include "GUtils.h"
#include "GLheaders.h"

// System includes
#include <string>


// Font class:
// This class holds a TTF font to be used to render text to the screen.
class Font
{
    public:
        // Constructor by loading font from TTF
        Font
           (FT_Library   library,
            char        *pathToFont,
            std::string  fontName,
            int          size);

        ~Font();

        // ---- GETTERS AND SETTERS ----
        // Get font
        FT_Face getFont()
        {
            return _font;
        }

        // Get new line distance in pixels
        int getNewLineDist()
        {
            return _newLineDist_pix;
        }

        // Get size of the font
        int getSize()
        {
            return _size_pix;
        }


        // Set size of font
        void setSize(int size_pix);

    private:
        // Font data
        FT_Face _font;

        // Name of the font
        std::string _fontName;

        // Size of the font
        int _size_pix;

        // Distance between lines in pixels
        int _newLineDist_pix;
};


#endif // H_MODULES_TEXT_FONT_H
