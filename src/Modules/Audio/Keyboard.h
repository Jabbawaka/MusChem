#ifndef H_MODULES_AUDIO_KEYBOARD_H
#define H_MODULES_AUDIO_KEYBOARD_H


// Project include files
#include "Modules/Input/Input.h"

// System include files
#include <vector>

struct Key
{
    // Identifier
    ActionKey _id;

    // Frequency associated
    float freq_Hz;

    // Time pressed and released measured in frames
    int _timePress_frame;
    int _timeRelease_frame;

    // Flag indicating whether the key is pressed
    bool _isPressedFlag;

    // Flag indicating whether the key is on
    bool _isOnFlag;

    // ---- METHODS ----
    // Constructor
    Key(ActionKey id)
    {
        _id = id;

        freq_Hz = 0.0f;

        _timePress_frame = 0;
        _timeRelease_frame = 0;
        _isPressedFlag = false;
        _isOnFlag = false;
    }

    // Press key
    void press(int time_frame)
    {
        _timePress_frame = time_frame;
        _isPressedFlag = true;
        _isOnFlag = true;
    }

    // Release key
    void release(int time_frame)
    {
        _isPressedFlag = false;
        _timeRelease_frame = time_frame;
    }
};


class Keyboard
{
    public:
        // Constructor: hard coded for now
        Keyboard();

        // Update
        void update(int time_frame);

        // Get keys
        std::vector<Key> getKeys()
        {
            return _keys;
        }

    private:
        // Vector of keys
        std::vector<Key> _keys;

};

#endif // H_MODULES_AUDIO_KEYBOARD_H
