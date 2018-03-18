// Project include files
#include "GUtils.h"
#include "GLogger/GLogger.h"

#include "Systems/BasicModules.h"

#include "Modules/Graphics/Graph.h"
#include "Modules/Graphics/Slider.h"
#include "Modules/Graphics/DiscSlider.h"
#include "Modules/Graphics/Switch.h"
#include "Modules/Graphics/Shader.h"
#include "Modules/Audio/Envelope.h"
#include "Modules/Audio/Keyboard.h"

// System include files
#include <stdio.h>
#include <vector>
#include "portaudio.h"


#define SAMPLE_RATE (44100)


struct sineWave
{
    Keyboard *p_keyboard;
    float beta;
    float freqVar;
    float lastFreqVar;
    float numModRatio;
    float denModRatio;

    // Filter stuff
    float cutOffFreq_Hz;
    float qParam_1;
    float omega;
    float bet;
    float gamma;

    float a0, a1, a2, b1, b2;
    float x_n1, x_n2, y_n1, y_n2;


    Envelope volEnv;
    Envelope betEnv;

    unsigned int frameTime;

    sineWave()
    {
        beta = 0.0f;
        numModRatio = 0.0f;
        denModRatio = 1.0f;
        frameTime = 0;

        // Filter
        cutOffFreq_Hz = 8000.0f;
        qParam_1 = sqrt(2.0f);
        omega = 2.0f * PI * cutOffFreq_Hz / (float)SAMPLE_RATE;
        bet = ((1.0f - qParam_1 / 2.0f * sin(omega)) / (1.0f + qParam_1 / 2.0f * sin(omega))) / 2.0f;
        gamma = (0.5f + bet) * cos(omega);
        
        a0 = (0.5f + bet + gamma) / 2.0f;
        a1 = 0.5f + bet - gamma;
        a2 = a0;
        b1 = -2 * gamma;
        b2 = 2 * bet;

        x_n1 = 0.0f;
        x_n2 = 0.0f;
        y_n1 = 0.0f;
        y_n2 = 0.0f;
    }
};


typedef int PaStreamCallback
   (const void                      *p_input,
    void                            *p_output,
    unsigned long                    frameCount,
    const PaStreamCallbackTimeInfo  *p_timeInfo,
    PaStreamCallbackFlags            statusFlags,
    void                            *p_userData);

static int playSin
   (const void                      *p_inputBuffer,
    void                            *p_outputBuffer,
    unsigned long                    framesPerBuffer,
    const PaStreamCallbackTimeInfo  *p_timeInfo,
    PaStreamCallbackFlags            statusFlags,
    void                            *p_userData)
{
    /* Cast data passed through stream to our structure */
    sineWave *p_data = (sineWave *) p_userData;
    float *p_out = (float *) p_outputBuffer;

    (void) p_inputBuffer; /* Prevent unused variable warning */
    (void) p_timeInfo;
    (void) statusFlags;
    float out;
    int onKeys;
    int time;
    float time_s;
    float freq;
    float volume;
    float beta;
    float ratio;

    for(unsigned int i = 0; i < framesPerBuffer; i++)
    {
        time = p_data->frameTime;
        std::vector<Key> keys = p_data->p_keyboard->getKeys();

        if(p_data->denModRatio == 0.0f)
        {
            ratio = 0.0f;
        }
        else
        {
            ratio = p_data->numModRatio / p_data->denModRatio;
        }

        out = 0;
        onKeys = 0;

        for(unsigned int iKey = 0; iKey < keys.size(); iKey++)
        {
            if(keys[iKey]._isOnFlag == true)
            {
                if(keys[iKey]._isPressedFlag == true)
                {
                    time_s = ((float)(time - keys[iKey]._timePress_frame) / (float)SAMPLE_RATE);
                }
                else
                {
                    time_s = ((float)(time - keys[iKey]._timeRelease_frame) / (float)SAMPLE_RATE);
                }

                volume = p_data->volEnv.getValue(time_s, keys[iKey]._isPressedFlag);
                beta = p_data->beta * p_data->betEnv.getValue(time_s, keys[iKey]._isPressedFlag);

                if(volume < 0.0f)
                {
                    keys[iKey]._isOnFlag = false;
                }
                else
                {
                    freq = keys[iKey].freq_Hz;
                    out += volume * sin
                       (2.0f * PI * freq * time / (float)SAMPLE_RATE +
                        beta * sin(ratio * 2.0f * PI * time * freq / (float)SAMPLE_RATE));
                    onKeys++;
                }
            }
        }

        if(onKeys > 0)
        {
            out /= (float) onKeys;
        }
        else
        {
            out = 0.0f;
        }

        // Calculate filter
        float filterOut =
            p_data->a0 * out + p_data->a1 * p_data->x_n1 + p_data->a2 * p_data->x_n2 -
            p_data->b1 * p_data->y_n1 - p_data->b2 * p_data->y_n2;

        p_data->y_n2 = p_data->y_n1;
        p_data->y_n1 = filterOut;
        p_data->x_n2 = p_data->x_n1;
        p_data->x_n1 = out;
        out = filterOut;

        if(out > 1.0f)
        {
            out = 1.0f;
        }
        else if(out < -1.0f)
        {
            out = -1.0f;
        }
        
        p_data->frameTime++;

        *p_out++ = out;
        *p_out++ = out;
    }

    return paContinue;
}


int main(int argc, char *argv[])
{
    // ---- INITIALISATION ----
    // Portaudio
    GLOG_MSG("Initialising PortAudio... ");
    PaError error = Pa_Initialize();
    if(error != paNoError)
    {
        GLOG_ERR("Could not initialise PortAudio: %s", Pa_GetErrorText(error));
        return 1;
    }

    // Keyboard
    Keyboard keyboard;

    // Wave stuff
    sineWave data;
    data.p_keyboard = &keyboard;
    data.beta = 0.0f;
    data.freqVar = 1.0f;
    data.lastFreqVar = 1.0f;
    data.numModRatio = 0.0f;
    data.denModRatio = 1.0f;
    data.frameTime = 0;

    // Envelopes
    std::vector<glm::vec2> volPoints;
    volPoints.push_back(glm::vec2(0.0f, 0.0f));
    volPoints.push_back(glm::vec2(0.2f, 0.8f));
    volPoints.push_back(glm::vec2(0.3f, 0.4f));
    volPoints.push_back(glm::vec2(1.0f, 0.4f));
    data.volEnv.setData(volPoints, 0.1f);

    std::vector<glm::vec2> betaPoints;
    betaPoints.push_back(glm::vec2(0.0f, 1.0f));
    betaPoints.push_back(glm::vec2(0.5f, 1.0f));
    betaPoints.push_back(glm::vec2(1.0f, 1.0f));
    data.betEnv.setData(betaPoints, 0.1f);

    // ---- GRAPHICS ----
    // Sliders
    Slider betSlider
       (&data.beta, false,
        glm::vec2(-160.0f, 0.0f), 110.0f, glm::vec2(0.0f, 10.0f));
    DiscSlider numModSlider
       (&data.numModRatio,
        glm::vec2(-135.0f, 0.0f), 110.0f, glm::vec2(0.0f, 10.0f), 1.0f);
    DiscSlider denModSlider
    (&data.denModRatio,
        glm::vec2(-110.0f, 0.0f), 110.0f, glm::vec2(0.0f, 10.0f), 1.0f);

    // Graphs
    Graph volGraph
       (data.volEnv.getPoints(),
        glm::vec2(-380.0f, 120.0f), glm::vec2(200.0f, 110.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
        glm::vec2(0.125f, 0.25f));
    Graph betaGraph
       (data.betEnv.getPoints(),
        glm::vec2(-380.0f, 0.0f), glm::vec2(200.0f, 110.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
        glm::vec2(0.125f, 0.25f));

    // Switches
    Switch volSwitch(data.volEnv.getLoop(), glm::vec2(-410.0f, 200.0f));
    Switch betSwitch(data.betEnv.getLoop(), glm::vec2(-410.0f, 80.0f));

    // Open an audio I/O stream
    GLOG_MSG("Opening IO stream... ");
    PaStream *p_sinStream;
    error = Pa_OpenDefaultStream
       (&p_sinStream,
        0,           // No input channels
        2,           // Stereo ouptut
        paFloat32,   // 32 bit floating point output
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,         // frames per buffer
        playSin, // Callback function
        &data);      // Pointer to data passed to the callback
    if(error != paNoError)
    {
        GLOG_ERR("Could not open an IO stream: %s", Pa_GetErrorText(error));
        return 1;
    }

    Pa_StartStream(p_sinStream);

    do
    {
        input.processInputs();

        // ---- UPDATE ----
        keyboard.update(data.frameTime);

        betSlider.update();
        numModSlider.update();
        denModSlider.update();

        volGraph.update();
        betaGraph.update();

        volSwitch.update();
        betSwitch.update();

        // ---- RENDER ----
        graphics.beginRender();

        // This bit is quite hacky
        for(unsigned int iKey = 0; iKey < keyboard.getKeys().size(); iKey++)
        {
            Key key = keyboard.getKeys()[iKey];
            if(key._isPressedFlag == true)
            {
                float xTime_s =
                    (float)(data.frameTime - key._timePress_frame) /
                    (float)SAMPLE_RATE;

                float volTime_s = xTime_s;
                float betTime_s = xTime_s;

                if(data.volEnv.getLoop() == true)
                {
                    while(volTime_s > 1.0f)
                    {
                        volTime_s -= 1.0f;
                    }
                }
                if(data.betEnv.getLoop() == true)
                {
                    while(betTime_s > 1.0f)
                    {
                        betTime_s -= 1.0f;
                    }
                }

                if(volTime_s > 1.0f)
                {
                    volTime_s = 1.0f;
                }
                if(betTime_s > 1.0f)
                {
                    betTime_s = 1.0f;
                }

                volGraph.renderVert(volTime_s);
                betaGraph.renderVert(betTime_s);
            }
        }
        volGraph.render(glm::vec3(0.2f, 0.5f, 0.2f));
        betaGraph.render(glm::vec3(0.2f, 0.5f, 0.2f));

        betSlider.render();
        numModSlider.render();
        denModSlider.render();

        volSwitch.render();
        betSwitch.render();

        graphics.endRender();

    } while(input.wasKeyPressed(EXIT) == false);

    /* Close PortAudio */
    Pa_StopStream(p_sinStream);
    error = Pa_CloseStream(p_sinStream);

    GLOG_MSG("Closing PortAudio... ");
    error = Pa_Terminate();
    if(error != paNoError)
    {
        GLOG_ERR("\nCould not close PortAudio: %s", Pa_GetErrorText(error));
        return 1;
    }

    return 0;
}
