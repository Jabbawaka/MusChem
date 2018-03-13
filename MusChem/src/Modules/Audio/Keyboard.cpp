// Self include files
#include "Modules/Audio/Keyboard.h"

// Project include files
#include "Modules/Input/Input.h"

// System include files
#include "Systems/BasicModules.h"


Keyboard::Keyboard()
{
    float freq_Hz = 220.0f;

    for(int iKey = C; iKey <= B; iKey++)
    {
        _keys.push_back(Key((ActionKey) iKey));
        _keys.back().freq_Hz = freq_Hz;

        freq_Hz += 220.0f / 12.0f;
    }
}

void Keyboard::update(int time_frame)
{
    for(int iKey = C; iKey <= B; iKey++)
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