// Self include files
#include "GUtils.h"
#include "Modules/Audio/Envelope.h"

// Project include files

// System include files
#include <vector>


Envelope::Envelope()
{
    _timePress_s = 0.0f;
    _timeRelease_s = 0.0f;
    _currValue = 0.0f;

    _points.push_back(glm::vec2(0.0f, 0.0f));
    _points.push_back(glm::vec2(1.0f, 0.0f));
    _nextPoint = 1;
}

void Envelope::setData
   (std::vector<glm::vec2> points)
{
    _points = points;
    _nextPoint = 1;
}

void Envelope::press(float time_s)
{
    _timePress_s = time_s;
    _isPressedFlag = true;
}

void Envelope::release(float time_s)
{
    _timeRelease_s = time_s;
    _isPressedFlag = false;
    _releaseValue = _currValue;
}

void Envelope::update(float time_s)
{
    // Time delta from key action
    float timeDelta_s;

    if(_isPressedFlag == true)
    {
        timeDelta_s = time_s - _timePress_s;

        if(timeDelta_s > _points[_points.size() - 1].x)
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
        timeDelta_s = time_s - _timeRelease_s;
        _currValue = _releaseValue * (1 - timeDelta_s / _DECAY_TIME);
        _nextPoint = 1;
    }

    if(_currValue < 0.0f)
    {
        _currValue = 0.0f;
    }
}

float Envelope::getValue()
{
    return _currValue;
}

void Envelope::render()
{
    _graph.render(_points, glm::vec3(1.0f, 0.2f, 0.2f));
}

std::vector<glm::vec2> *getPoints()
{
    return &_points;
}
