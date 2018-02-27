#include <stdio.h>
#include "portaudio.h"
#include "GUtils.h"
#include "GLogger/GLogger.h"
#include "Systems/BasicModules.h"


#define SAMPLE_RATE (44100)

typedef struct
{
    float peakTime_s;
    float sustTime_s;
    float relTime_s;

    float timePress_s;
    float timeRelease_s;

    bool isPressed;

    float peakLevel;
    float sustLevel;

    float releaseLevel;

    void start()
    {
        timePress_s = 0.0f;
        timeRelease_s = 0.0f;
    }

    float getValue(float inTime_s)
    {
        float value = 0.0f;
        float time_s = inTime_s - timePress_s;

        if(isPressed == true)
        {
            if(peakTime_s < 0.0f)
            {
                value = sustLevel;
            }
            else
            {
                if(time_s <= peakTime_s)
                {
                    value = time_s / peakTime_s * peakLevel;
                }
                else if(time_s < peakTime_s + sustTime_s)
                {
                    value = peakLevel + (time_s - peakTime_s) / sustTime_s * (sustLevel - peakLevel);
                }
                else
                {
                    value = sustLevel;
                }

                if(value < 0.0f)
                {
                    value = 0.0f;
                }
            }
        }
        else
        {
            // Do release
            value = (1 - (inTime_s - timeRelease_s) / 0.1f) * releaseLevel;

            if(value < 0.0f)
            {
                value = 0.0f;
            }
        }

        return value;
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
        volEnv.timePress_s = (float)frameTime / (float)SAMPLE_RATE;
        volEnv.isPressed = true;
        betEnv.timePress_s = (float)frameTime / (float)SAMPLE_RATE;
        betEnv.isPressed = true;
        modEnv.timePress_s = (float)frameTime / (float)SAMPLE_RATE;
        modEnv.isPressed = true;
    }

    void release()
    {
        volEnv.timeRelease_s = (float)frameTime / (float)SAMPLE_RATE;
        volEnv.releaseLevel = volEnv.getValue(volEnv.timeRelease_s);
        volEnv.isPressed = false;
        betEnv.timeRelease_s = (float)frameTime / (float)SAMPLE_RATE;
        betEnv.releaseLevel = betEnv.getValue(betEnv.timeRelease_s);
        betEnv.isPressed = false;
        modEnv.timeRelease_s = (float)frameTime / (float)SAMPLE_RATE;
        modEnv.releaseLevel = modEnv.getValue(modEnv.timeRelease_s);
        modEnv.isPressed = false;
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
    float freq;
    float volume;
    float beta;
    float ratio;

    for(unsigned int i = 0; i < framesPerBuffer; i++)
    {
        /* ---- GET DATA FROM INPUT ---- */
        freq = p_data->freq_Hz;
        time = p_data->frameTime;
        volume = p_data->volEnv.getValue((float)time / (float)SAMPLE_RATE);
        beta = p_data->beta * p_data->betEnv.getValue((float)time / (float)SAMPLE_RATE);
        ratio = p_data->modRatio * p_data->modEnv.getValue((float)time / (float)SAMPLE_RATE);

        out = volume * sin
           (2 * PI * freq * time / (float)SAMPLE_RATE +
            beta * sin(0.5f * 2 * PI * time * freq / (float)SAMPLE_RATE));

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
    /* Initialise PortAudio */
    printf("Initialising PortAudio... ");
    PaError error = Pa_Initialize();
    if(error != paNoError)
    {
        printf("\nCould not initialise PortAudio: %s\n", Pa_GetErrorText(error));
        return 1;
    }
    printf("Success!\n");

    /* ---- SET UP DATA ---- */
    /* Basic stuff */
    sineWave data;
    data.freq_Hz = 0.0f;
    data.beta = 1.0f;
    data.modRatio = 1.5f;
    data.frameTime = 0;

    /* Volume envelope */
    data.volEnv.peakLevel = 0.8f;
    data.volEnv.sustLevel = 0.6f;
    data.volEnv.peakTime_s = 0.8f;
    data.volEnv.sustTime_s = 0.2f;
    data.volEnv.relTime_s = 0.2f;
    data.volEnv.start();

    /* Beta envelope */
    data.betEnv.peakLevel = 1.0f;
    data.betEnv.sustLevel = 1.0f;
    data.betEnv.peakTime_s = 0.4f;
    data.betEnv.sustTime_s = 0.2f;
    data.betEnv.relTime_s = 1.0f;
    data.betEnv.start();

    /* Ratio envelope */
    data.modEnv.peakLevel = 5.0f;
    data.modEnv.sustLevel = 1.0f;
    data.modEnv.peakTime_s = 0.4f;
    data.modEnv.sustTime_s = 0.2f;
    data.modEnv.relTime_s = 1.0f;
    data.modEnv.start();

    /* Open an audio I/O stream */
    printf("Opening IO stream... ");
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
        printf("\nCould not open an IO stream: %s\n", Pa_GetErrorText(error));
        return 1;
    }
    printf("Success!\n");

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

    } while(input.wasKeyPressed(EXIT) == false);

    /* Close PortAudio */
    Pa_StopStream(p_sinStream);
    error = Pa_CloseStream(p_sinStream);

    printf("Closing PortAudio... ");
    error = Pa_Terminate();
    if(error != paNoError)
    {
        printf("\nCould not close PortAudio: %s\n", Pa_GetErrorText(error));
        return 1;
    }
    printf("Success!\n");

    return 0;
}
