// Self include files
#include "GUtils.h"
#include "Modules/Audio/Envelope.h"

// Project include files
#include "GLogger/GLogger.h"

// System include files
#include <vector>


Envelope::Envelope()
{
    _currValue = 0.0f;

    _points.push_back(glm::vec2(0.0f, 0.0f));
    _points.push_back(glm::vec2(1.0f, 0.0f));
    _nextPoint = 1;
    _loopEnabledFlag = false;
}

void Envelope::setData
   (std::vector<glm::vec2> points, float decayTime_s)
{
    _points = points;
    _nextPoint = 1;
    _decay_s = decayTime_s;
}

float Envelope::getValue(float timeDelta_s, bool pressed)
{
    if(_loopEnabledFlag == true)
    {
        _points.back().y = _points.front().y;
        // Consume all exceeding time
        while(timeDelta_s > _points.back().x - _points.front().x)
        {
            timeDelta_s -= _points.back().x - _points.front().x;
            _nextPoint = 0;
        }
    }

    if(pressed == true)
    {
        if(timeDelta_s > _points[_points.size() - 2].x)
        {
            // Greater than latest point, output sustain value
            _nextPoint = _points.size() - 1;
            _currValue = _points[_nextPoint].y;
        }
        else
        {
            // Somewhere in the middle, interpolation time
            while(timeDelta_s > _points[_nextPoint].x)
            {
                _nextPoint++;
            }
            float xPrev, xNext, yPrev, yNext;
            xPrev = _points[_nextPoint - 1].x;
            yPrev = _points[_nextPoint - 1].y;
            xNext = _points[_nextPoint].x;
            yNext = _points[_nextPoint].y;

            _currValue = yPrev + (yNext - yPrev) * (timeDelta_s - xPrev) / (xNext - xPrev);
        }
    }
    else
    {
        // Linearly interpolate based on decay time setting
        _currValue = _releaseValue * (1 - timeDelta_s / _decay_s);
        _nextPoint = 1;
    }

    return _currValue;
}