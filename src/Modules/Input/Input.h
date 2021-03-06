#ifndef H_MODULES_INPUT_INPUT_H
#define H_MODULES_INPUT_INPUT_H


// Project includes
#include "Modules/Input/Command.h"
#include "Modules/Graphics/Window.h"

// System includes


// Action enum:
// Enum to specify all the actions associated with key presses.
enum ActionKey
{
    F2,
    FS2,
    G2,
    GS2,
    A2,
    AS2,
    B2,
    C3,
    CS3,
    D3,
    DS3,
    E3,
    F3,
    FS3,
    G3,
    GS3,
    A3,
    AS3,
    B3,
    C4,
    CS4,
    D4,
    DS4,
    E4,
    F4,
    FS4,
    G4,
    GS4,
    A4,
    AS4,
    B4,
    C5,
    CS5,
    D5,
    DS5,
    E5,
    F5,
    EXIT,
    NUM_ACTION_KEYS
};


// Input class:
// This class defines the Input module, which handles all input events and is
// responsible for initialising the SDL input subsystem.
class Input
{
    public:
        // Constructor: initialises GLFW events, needs a pointer to the window
        Input(const char *pathToFile);

        // Destructor: closes SDL
        ~Input();

        // Process inputs: must be called once per frame
        void processInputs();

        // Query if key is down: this function should be called after
        // processInputs has been called for the frame.
        bool isKeyDown(ActionKey key);

        // Query if key was pressed this frame: this function should be called
        // after processInputs has been called for the frame.
        bool wasKeyPressed(ActionKey key);

        // Query if key was released this frame: this function should be called
        // after processInputs has been called for the frame.
        bool wasKeyReleased(ActionKey key);

        // Query if the left mouse button is down: this function should be called
        // after processInputs has been called for the frame.
        bool isLeftMouseDown();

        // Query if the right mouse button is down: this function should be called
        // after processInputs has been called for the frame.
        bool isRightMouseDown();

        // Query if the mouse was pressed: this function should be called after
        // processInputs has been called for the frame.
        bool wasLeftMousePressed();

        // Query if the mouse was pressed: this function should be called after
        // processInputs has been called for the frame.
        bool wasRightMousePressed();

        // Query if the mouse was clicked: this function should be called after
        // processInputs has been called for the frame.
        bool wasLeftMouseClicked();

        // Query if the mouse was clicked: this function should be called after
        // processInputs has been called for the frame.
        bool wasRightMouseClicked();

        // Load controls from file
        void loadControls(const char *pathToFile);

        // Save controls to file
        bool saveControls(const char *pathToFile);

        // Get displacement of mouse
        glm::vec2 getMousePos()
        {
            return _mousePos_pix_glob;
        }

    private:
        // Window to process inputs
        Window *_p_window;

        // Array with keys
        Command _keyArray[NUM_ACTION_KEYS];

        // Mouse variables
        glm::vec2 _mousePos_pix_glob;
        bool _leftClicked;
        bool _leftDown;
        bool _leftPressed;
        bool _rightClicked;
        bool _rightDown;
        bool _rightPressed;

};

#endif // H_MODULES_INPUT_INPUT_H
