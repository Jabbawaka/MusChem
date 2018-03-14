// Project include files
#include "GUtils.h"
#include "GLogger/GLogger.h"

#include "Systems/BasicModules.h"

#include "Modules/Graphics/Graph.h"
#include "Modules/Graphics/Slider.h"
#include "Modules/Graphics/DiscSlider.h"
#include "Modules/Graphics/Shader.h"
#include "Modules/Audio/Envelope.h"
#include "Modules/Audio/Keyboard.h"

// System include files
#include <stdio.h>
#include <vector>
#include "portaudio.h"


#define SAMPLE_RATE (44100)


typedef struct
{
    Keyboard *p_keyboard;
    float beta;
    float numModRatio;
    float denModRatio;

    Envelope volEnv;
    Envelope betEnv;

    unsigned int frameTime;

} sineWave;


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
       (&data.beta,
        glm::vec2(-190.0f, 0.0f), 110.0f, glm::vec2(0.0f, 10.0f));
    DiscSlider numModSlider
       (&data.numModRatio,
        glm::vec2(-190.0f, -120.0f), 110.0f, glm::vec2(0.0f, 10.0f), 1.0f);
    DiscSlider denModSlider
       (&data.denModRatio,
        glm::vec2(-165.0f, -120.0f), 110.0f, glm::vec2(0.0f, 10.0f), 1.0f);

    // Graphs
    Graph volGraph
       (data.volEnv.getPoints(),
        glm::vec2(-410.0f, 120.0f), glm::vec2(200.0f, 110.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
        glm::vec2(0.2f, 0.25f));
    Graph betaGraph
       (data.betEnv.getPoints(),
        glm::vec2(-410.0f, 0.0f), glm::vec2(200.0f, 110.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
        glm::vec2(0.2f, 0.25f));

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

        // ---- RENDER ----
        graphics.beginRender();

        volGraph.render(glm::vec3(0.2f, 0.5f, 0.2f));
        betaGraph.render(glm::vec3(0.2f, 0.5f, 0.2f));

        betSlider.render();
        numModSlider.render();
        denModSlider.render();

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
