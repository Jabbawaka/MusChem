#include <stdio.h>
#include "portaudio.h"
#include "GUtils.h"
#include "GLogger/GLogger.h"
#include "Systems/BasicModules.h"
#include "Modules/Graphics/Graph.h"
#include "Modules/Graphics/Shader.h"
#include <vector>


#define SAMPLE_RATE (44100)

typedef struct
{
    const float _DECAY_TIME = 0.2f;
    float timePress_s;
    float timeRelease_s;
    float releaseValue;
    float currValue;

    bool isPressed;

    std::vector<glm::vec2> envPoints;
    int currNextPoint;

    void start(std::vector<glm::vec2> points)
    {
        timePress_s = 0.0f;
        timeRelease_s = 0.0f;
        currValue = 0.0f;

        envPoints = points;
        currNextPoint = 1;
    }

    float getValue()
    {
        return currValue;
    }

    void press(float time_s)
    {
        isPressed = true;
        timePress_s = time_s;
    }

    void release(float time_s)
    {
        isPressed = false;
        timeRelease_s = time_s;
        releaseValue = currValue;
    }

    void update(float time_s)
    {
        // Time delta from key action
        float timeDelta_s;

        if(isPressed == true)
        {
            timeDelta_s = time_s - timePress_s;

            if(timeDelta_s > envPoints[envPoints.size() - 1].x)
            {
                // Greater than latest point, so at sustain value
                currNextPoint = envPoints.size() - 1;
                currValue = envPoints[currNextPoint].y;
            }
            else
            {
                // Somewhtere in the middle, interpolation time
                while(timeDelta_s > envPoints[currNextPoint].x)
                {
                    currNextPoint++;
                }

                float xPrev, xNext, yPrev, yNext;
                xPrev = envPoints[currNextPoint - 1].x;
                yPrev = envPoints[currNextPoint - 1].y;
                xNext = envPoints[currNextPoint].x;
                yNext = envPoints[currNextPoint].y;

                currValue = yPrev + (yNext - yPrev) * (timeDelta_s - xPrev) / (xNext - xPrev);
            }
        }
        else
        {
            // Linearly interpolate based on decay time setting
            timeDelta_s = time_s - timeRelease_s;
            currValue = releaseValue * (1 - timeDelta_s / _DECAY_TIME);
            currNextPoint = 1;
        }

        if(currValue < 0.0f)
        {
            currValue = 0.0f;
        }
    }
} envelope;

typedef struct
{
    float freq_Hz;
    float beta;
    float modRatio;

    envelope volEnv;
    envelope betEnv;
    envelope modEnv;

    unsigned int frameTime;

    void setTime()
    {
        float time_s = (float)frameTime / (float)SAMPLE_RATE;

        volEnv.press(time_s);
        betEnv.press(time_s);
        modEnv.press(time_s);
    }

    void release()
    {
        float time_s = (float)frameTime / (float)SAMPLE_RATE;

        volEnv.release(time_s);
        betEnv.release(time_s);
        modEnv.release(time_s);
    }
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
    int time;
    float time_s;
    float freq;
    float volume;
    float beta;
    float ratio;

    for(unsigned int i = 0; i < framesPerBuffer; i++)
    {
        /* ---- GET DATA FROM INPUT ---- */
        freq = p_data->freq_Hz;
        time = p_data->frameTime;
        
        time_s = (float)time / (float)SAMPLE_RATE;
        p_data->volEnv.update(time_s);
        p_data->betEnv.update(time_s);
        p_data->modEnv.update(time_s);
        
        volume = p_data->volEnv.getValue();
        beta = p_data->beta * p_data->betEnv.getValue();
        ratio = p_data->modRatio * p_data->modEnv.getValue();

        out = volume * sin
           (2 * PI * freq * time / (float)SAMPLE_RATE +
            beta * sin(0.5f * 2 * PI * time * freq / (float)SAMPLE_RATE));

        p_data->frameTime++;

        *p_out++ = out;
        *p_out++ = out;
    }

    return paContinue;
}


int main(int argc, char *argv[])
{
    GLuint vbo;
    glGenBuffers(1, &vbo);
    /* Initialise PortAudio */
    GLOG_MSG("Initialising PortAudio... ");
    PaError error = Pa_Initialize();
    if(error != paNoError)
    {
        GLOG_ERR("\nCould not initialise PortAudio: %s", Pa_GetErrorText(error));
        return 1;
    }
    GLOG_MSG("Success!");

    /* ---- SET UP DATA ---- */
    /* Basic wave stuff */
    sineWave data;
    data.freq_Hz = 0.0f;
    data.beta = 1.0f;
    data.modRatio = 0.5f;
    data.frameTime = 0;

    /* Volume envelope */
    std::vector<glm::vec2> volPoints;
    volPoints.push_back(glm::vec2(0.0f, 0.0f));
    volPoints.push_back(glm::vec2(0.2f, 0.7f));
    volPoints.push_back(glm::vec2(0.3f, 0.4f));
    volPoints.push_back(glm::vec2(0.5f, 0.8f));
    volPoints.push_back(glm::vec2(0.6f, 0.6f));
    volPoints.push_back(glm::vec2(0.8f, 0.4f));
    volPoints.push_back(glm::vec2(1.0f, 0.3f));
    data.volEnv.start(volPoints);

    Graph volGraph
       (glm::vec2(-410.0f, -230.0f), glm::vec2(400.0f, 220.0f),
        glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 1.0f));

    /* Beta envelope */
    std::vector<glm::vec2> betaPoints;
    betaPoints.push_back(glm::vec2(0.0f, 1.0f));
    betaPoints.push_back(glm::vec2(0.2f, 3.0f));
    betaPoints.push_back(glm::vec2(0.4f, 1.0f));
    data.betEnv.start(betaPoints);

    /* Ratio envelope */
    std::vector<glm::vec2> ratioPoints;
    ratioPoints.push_back(glm::vec2(0.0f, 1.0f));
    data.modEnv.start(ratioPoints);

    /* Open an audio I/O stream */
    GLOG_MSG("Opening IO stream... ");
    PaStream *p_sinStream;
    error = Pa_OpenDefaultStream
       (&p_sinStream,
        0,           /* No input channels */
        2,           /* Stereo ouptut */
        paFloat32,   /* 32 bit floating point output */
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,         /* frames per buffer */
        playSin, /* Callback function */
        &data);      /* Pointer to data passed to the callback */
    if(error != paNoError)
    {
        GLOG_ERR("\nCould not open an IO stream: %s", Pa_GetErrorText(error));
        return 1;
    }
    GLOG_MSG("Success!");

    Pa_StartStream(p_sinStream);

    do
    {
        input.processInputs();

        if(input.isKeyDown(A) == true)
        {
            data.freq_Hz = 220.0f;
            if(input.wasKeyPressed(A) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(A_S) == true)
        {
            data.freq_Hz = 233.08f;
            if(input.wasKeyPressed(A_S) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(B) == true)
        {
            data.freq_Hz = 246.94f;
            if(input.wasKeyPressed(B) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(C) == true)
        {
            data.freq_Hz = 130.81f;
            if(input.wasKeyPressed(C) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(C_S) == true)
        {
            data.freq_Hz = 138.59f;
            if(input.wasKeyPressed(C_S) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(D) == true)
        {
            data.freq_Hz = 146.83f;
            if(input.wasKeyPressed(D) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(D_S) == true)
        {
            data.freq_Hz = 155.56f;
            if(input.wasKeyPressed(D_S) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(E) == true)
        {
            data.freq_Hz = 164.81f;
            if(input.wasKeyPressed(E) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(F) == true)
        {
            data.freq_Hz = 174.61f;
            if(input.wasKeyPressed(F) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(F_S) == true)
        {
            data.freq_Hz = 185.0f;
            if(input.wasKeyPressed(F_S) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(G) == true)
        {
            data.freq_Hz = 196.00f;
            if(input.wasKeyPressed(G) == true)
            {
                data.setTime();
            }
        }
        else if(input.isKeyDown(G_S) == true)
        {
            data.freq_Hz = 207.65f;
            if(input.wasKeyPressed(G_S) == true)
            {
                data.setTime();
            }
        }

        if(input.wasKeyReleased(A) == true ||
           input.wasKeyReleased(A_S) == true ||
           input.wasKeyReleased(B) == true ||
           input.wasKeyReleased(C) == true ||
           input.wasKeyReleased(C_S) == true ||
           input.wasKeyReleased(D) == true ||
           input.wasKeyReleased(D_S) == true ||
           input.wasKeyReleased(E) == true ||
           input.wasKeyReleased(F) == true ||
           input.wasKeyReleased(F_S) == true ||
           input.wasKeyReleased(G) == true ||
           input.wasKeyReleased(G_S) == true)
        {
            data.release();
        }

        data.beta = 0;
        if(input.isKeyDown(O0) == true)
        {
            data.beta = 1 / 8;
        }
        else if(input.isKeyDown(O1) == true)
        {
            data.beta = 1 / 4;
        }
        else if(input.isKeyDown(O2) == true)
        {
            data.beta = 1/ 2;
        }
        else if(input.isKeyDown(O3) == true)
        {
            data.beta = 0;
        }
        else if(input.isKeyDown(O4) == true)
        {
            data.beta = 1;
        }
        else if(input.isKeyDown(O5) == true)
        {
            data.beta = 2;
        }
        else if(input.isKeyDown(O6) == true)
        {
            data.beta = 3;
        }
        else if(input.isKeyDown(O7) == true)
        {
            data.beta = 4;
        }
        else if(input.isKeyDown(O8) == true)
        {
            data.beta = 5;
        }

        // ---- RENDER ----
        graphics.beginRender();

        volGraph.render(volPoints, glm::vec3(1.0f, 0.1f, 0.1f));

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
