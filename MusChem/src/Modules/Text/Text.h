#ifndef H_MODULES_TEXT_TEXT_H
#define H_MODULES_TEXT_TEXT_H


// Project includes
#include "GLheaders.h"
#include "GList/GList.h"

#include "Modules/Text/Font.h"

// System includes
#include <stdio.h>


// Constant definitions
#define TEXT_MAX_NUM_OF_FONTS (10)


// Text class:
// This class is responsible for initialising all libraries used by the text
// system, and it's the access point to the text rendering functionalities.
class Text
{
    public:
        // Constructor: initialises the Freetype library
        Text(const char *pathToFile);

        // Destructor: closes all open fonts and shuts down the Freetype library
        ~Text();

        // Get font
        Font *getFont(const char *fontName);

    private:
        // ---- FIELDS ----
        // Freetype library instance
        FT_Library _freeTypeLib;

        // Array of fonts
        GList<Font *> _fontList;

        // ---- METHODS ----
        void parseFile(const char *pathToFile);

};


#endif // H_MODULES_TEXT_TEXT_H
