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
        void setData(std::vector<glm::vec2> points);

        // Pressed key
        void press(float time_s);

        // Released key
        void release(float time_s);

        // Update status of envelope
        void update(float time_s);

        // Get current value of envelope
        float getValue();

        // Get points of the envelope
        std::vector<glm::vec2> &getPoints();

    private:
        // Decay time (maybe will be settable in the future)
        const float _DECAY_TIME = 0.2f;

        // Times of press and release
        float _timePress_s;
        float _timeRelease_s;

        // Value at release
        float _releaseValue;

        // Current value of the envelope
        float _currValue;

        // Variable to keep track of whether the key is pressed or not
        bool _isPressedFlag;

        // Points defining the envelope
        std::vector<glm::vec2> _points;
        int _nextPoint;

};
