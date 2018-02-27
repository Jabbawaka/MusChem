// Self include files
#include "Modules/Input/Input.h"

// Project include files
#include "GLogger/GLogger.h"
#include "GParser/GParser.h"
#include "GLheaders.h"

#include "Systems/BasicModules.h"

// System include files
#include <stdlib.h>
#include <string.h>


Input::Input(const char *pathToFile)
{
    // Initialise GLFW
    if(!glfwInit())
    {
        GLOG_ERR("Could not initialise GLFW");
        return;
    }

    // Store window pointer
    _p_window = &graphics.getWindow();

    // Initialise GLFW input
    glfwSetInputMode(_p_window->getGlfwWindow(), GLFW_STICKY_KEYS, 1);

    // Hide cursor
    glfwSetInputMode(_p_window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Set cursor to centre of screen
    glfwSetCursorPos
       (_p_window->getGlfwWindow(),
        _p_window->getWidth() / 2, _p_window->getHeight() / 2);

    // Load controls
    try
    {
        input.loadControls(pathToFile);
    }
    catch(...)
    {
        GLOG_ERR("Could not load controls from %s", pathToFile);
        throw;
    }

    GLOG_MSG("Initialised Input system");
}

Input::~Input()
{
}

void Input::processInputs()
{
    // ---- RESET ----
    // Reset was key pressed flag
    for(int i = 0; i < NUM_ACTION_KEYS; i++)
    {
        _keyArray[i].wasKeyPressed = false;
        _keyArray[i].wasKeyReleased = false;
        _leftClicked = false;
    }

    // Poll all events
    glfwPollEvents();

    // Catch exiting window
    if(glfwWindowShouldClose(_p_window->getGlfwWindow()) == true)
    {
        _keyArray[EXIT].wasKeyPressed = true;
    }

    // Iterate over keys setting them
    for(int i = 0; i < NUM_ACTION_KEYS; i++)
    {
        switch(glfwGetKey(_p_window->getGlfwWindow(), _keyArray[i].idGlfw))
        {
            case GLFW_PRESS:
                if(_keyArray[i].isKeyDown == false)
                {
                    _keyArray[i].wasKeyPressed = true;
                    _keyArray[i].isKeyDown = true;
                }
                break;
            case GLFW_RELEASE:
                if(_keyArray[i].isKeyDown == true)
                {
                    _keyArray[i].wasKeyReleased = true;
                }
                _keyArray[i].isKeyDown = false;
                break;
        }
    }

    // Get mouse buttons
    if(glfwGetMouseButton
       (_p_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        _leftDown = true;
    }
    else if(glfwGetMouseButton
       (_p_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        if(_leftDown == true)
        {
            _leftClicked = true;
        }
        _leftDown = false;
    }

    if(glfwGetMouseButton
       (_p_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        _rightDown = true;
    }
    else if(glfwGetMouseButton
       (_p_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        if(_rightDown == true)
        {
            _rightClicked = true;
        }
        _rightDown = false;
    }

    // Get mouse displacement
    double mouseXPos_pix_glob, mouseYPos_pix_glob;
    glfwGetCursorPos
       (_p_window->getGlfwWindow(),
        &mouseXPos_pix_glob, &mouseYPos_pix_glob);

    _mousePos_pix_glob.x = mouseXPos_pix_glob - (float) _p_window->getWidth() / 2.0f;
    _mousePos_pix_glob.y = -mouseYPos_pix_glob + (float) _p_window->getHeight() / 2.0f;
}

bool Input::isKeyDown(ActionKey key)
{
    return _keyArray[key].isKeyDown;
}

bool Input::wasKeyPressed(ActionKey key)
{
    return _keyArray[key].wasKeyPressed;
}

bool Input::wasKeyReleased(ActionKey key)
{
    return _keyArray[key].wasKeyReleased;
}
bool Input::wasLeftMouseClicked()
{
    return _leftClicked;
}

bool Input::wasRightMouseClicked()
{
    return _rightClicked;
}

void Input::loadControls(const char *pathToFile)
{
    ActionKey keyId;
    int glfwId;

    try
    {
        GParser parser(pathToFile);
        GParser_Line line;

        while( parser.readNextData(line) )
        {
            keyId = (ActionKey) atoi(line.key.c_str());
            glfwId = atoi(line.value.c_str());

            _keyArray[keyId].idGlfw = glfwId;
        }

        for(int i = 0; i < NUM_ACTION_KEYS; i++)
        {
            _keyArray[i].isKeyDown = false;
            _keyArray[i].wasKeyPressed = false;
        }

        GLOG_MSG("Controls correctly loaded from %s", pathToFile);
    }
    catch(...)
    {
        GLOG_ERR("Could not parse file %s", pathToFile);
        throw;
    }
}

bool Input::saveControls(const char *pathToFile)
{
    FILE *p_file = NULL;

    p_file = fopen(pathToFile, "w");
    if(p_file == NULL)
    {
        GLOG_ERR("Could not save controls file to %s", pathToFile);
        return false;
    }

    for(int iKey = 0; iKey < NUM_ACTION_KEYS; iKey++)
    {
        fprintf(p_file, "%d:%d\n", iKey, _keyArray[iKey].idGlfw);
    }

    GLOG_MSG("Controls correctly saved to %s", pathToFile);

    fclose(p_file);

    return true;
}
