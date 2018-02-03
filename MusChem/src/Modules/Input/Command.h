#ifndef H_MODULES_INPUT_COMMAND_H
#define H_MODULES_INPUT_COMMAND_H


// Project include files
#include "GLheaders.h"

// System include files


struct Command
{
    // SDL key ID
    int idGlfw;

    // Is key down?
    bool isKeyDown;

    // Is key pressed this frame?
    bool wasKeyPressed;
};


#endif // H_MODULES_INPUT_COMMAND_H
