// Project include files
#include "GUtils.h"

// System include files
#include <vector>


class Envelope
{
    public:
        // Default constructor
        Envelope();

        // Set data for the envelope
        void setData(std::vector<glm::vec2> points, float decayTime_s);

        // Get current value of envelope
        float getValue(float timeDelta_s, bool pressed);

        // Get points of the envelope
        std::vector<glm::vec2> &getPoints()
        {
            return _points;
        }

        // Get loop flag of the envelope
        bool &getLoop()
        {
            return _loopEnabledFlag;
        }

    private:
        // Decay time (maybe will be settable in the future)
        float _decay_s;

        // Value at release
        float _releaseValue;

        // Current value of the envelope
        float _currValue;

        // Variable to keep track of whether the key is pressed or not
        bool _isPressedFlag;

        // Flag to indicate whether the envelope should loop
        bool _loopEnabledFlag;

        // Points defining the envelope
        std::vector<glm::vec2> _points;
        int _nextPoint;

};
