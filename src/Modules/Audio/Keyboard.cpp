// Self include files
#include "Modules/Audio/Keyboard.h"

// Project include files
#include "Modules/Input/Input.h"

// System include files
#include "Systems/BasicModules.h"


Keyboard::Keyboard()
{
    _keys.push_back(Key((ActionKey) 0));
    _keys.back().freq_Hz = 87.31f;

    _keys.push_back(Key((ActionKey) 1));
    _keys.back().freq_Hz = 92.50f;

    _keys.push_back(Key((ActionKey) 2));
    _keys.back().freq_Hz = 98.00f;

    _keys.push_back(Key((ActionKey) 3));
    _keys.back().freq_Hz = 103.83f;

    _keys.push_back(Key((ActionKey) 4));
    _keys.back().freq_Hz = 110.00f;

    _keys.push_back(Key((ActionKey) 5));
    _keys.back().freq_Hz = 116.54f;

    _keys.push_back(Key((ActionKey) 6));
    _keys.back().freq_Hz = 123.47f;

    _keys.push_back(Key((ActionKey) 7));
    _keys.back().freq_Hz = 130.81f;

    _keys.push_back(Key((ActionKey) 8));
    _keys.back().freq_Hz = 138.59f;

    _keys.push_back(Key((ActionKey) 9));
    _keys.back().freq_Hz = 146.83f;

    _keys.push_back(Key((ActionKey) 10));
    _keys.back().freq_Hz = 155.56f;

    _keys.push_back(Key((ActionKey) 11));
    _keys.back().freq_Hz = 164.81f;

    _keys.push_back(Key((ActionKey) 12));
    _keys.back().freq_Hz = 174.61f;

    _keys.push_back(Key((ActionKey) 13));
    _keys.back().freq_Hz = 185.00f;

    _keys.push_back(Key((ActionKey) 14));
    _keys.back().freq_Hz = 196.00f;

    _keys.push_back(Key((ActionKey) 15));
    _keys.back().freq_Hz = 207.65f;

    _keys.push_back(Key((ActionKey) 16));
    _keys.back().freq_Hz = 220.00f;

    _keys.push_back(Key((ActionKey) 17));
    _keys.back().freq_Hz = 233.08f;

    _keys.push_back(Key((ActionKey) 18));
    _keys.back().freq_Hz = 246.94f;

    _keys.push_back(Key((ActionKey) 19));
    _keys.back().freq_Hz = 261.63f;

    _keys.push_back(Key((ActionKey) 20));
    _keys.back().freq_Hz = 277.18f;

    _keys.push_back(Key((ActionKey) 21));
    _keys.back().freq_Hz = 293.66f;

    _keys.push_back(Key((ActionKey) 22));
    _keys.back().freq_Hz = 311.13f;

    _keys.push_back(Key((ActionKey) 23));
    _keys.back().freq_Hz = 329.63f;

    _keys.push_back(Key((ActionKey) 24));
    _keys.back().freq_Hz = 349.23f;

    _keys.push_back(Key((ActionKey) 25));
    _keys.back().freq_Hz = 369.99f;

    _keys.push_back(Key((ActionKey) 26));
    _keys.back().freq_Hz = 392.00f;

    _keys.push_back(Key((ActionKey) 27));
    _keys.back().freq_Hz = 415.30f;

    _keys.push_back(Key((ActionKey) 28));
    _keys.back().freq_Hz = 440.00f;

    _keys.push_back(Key((ActionKey) 29));
    _keys.back().freq_Hz = 466.16f;

    _keys.push_back(Key((ActionKey) 30));
    _keys.back().freq_Hz = 493.88f;

    _keys.push_back(Key((ActionKey) 31));
    _keys.back().freq_Hz = 523.25f;

    _keys.push_back(Key((ActionKey) 32));
    _keys.back().freq_Hz = 554.37f;

    _keys.push_back(Key((ActionKey) 33));
    _keys.back().freq_Hz = 587.33f;

    _keys.push_back(Key((ActionKey) 34));
    _keys.back().freq_Hz = 622.25f;

    _keys.push_back(Key((ActionKey) 35));
    _keys.back().freq_Hz = 659.25f;

    _keys.push_back(Key((ActionKey) 36));
    _keys.back().freq_Hz = 698.46f;
}

void Keyboard::update(int time_frame)
{
    for(int iKey = 0; iKey < _keys.size(); iKey++)
    {
        if(input.wasKeyPressed((ActionKey)iKey) == true)
        {
            _keys[iKey].press(time_frame);
        }
        else if(input.wasKeyReleased((ActionKey)iKey) == true)
        {
            _keys[iKey].release(time_frame);
        }
    }
}
