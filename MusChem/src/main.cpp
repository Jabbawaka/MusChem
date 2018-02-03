#include <stdio.h>
#include "portaudio.h"
#include "GUtils.h"
#include "Systems/BasicModules.h"


#define SAMPLE_RATE (44100)

typedef struct
{
    float freq_Hz;
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
    float wave;

    for(unsigned int i = 0; i < framesPerBuffer; i++)
    {
        wave = 0.3f * sin
           (p_data->frameTime * 2 * PI * p_data->freq_Hz / (float) SAMPLE_RATE +
            3 * sin(p_data->frameTime * 2 * PI * 0.75 / (float) SAMPLE_RATE));
        if(wave > 1.0f)
        {
            wave = 1.0f;
        }
        else if(wave < -1.0f)
        {
            wave = -1.0f;
        }
        p_data->frameTime++;

        *p_out++ = wave;
        *p_out++ = wave;
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

    /* Open an audio I/O stream */
    printf("Opening IO stream... ");
    PaStream *p_sinStream;
    sineWave data;
    data.freq_Hz = 220.0f;
    data.frameTime = 0;
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

    do
    {
        input.processInputs();

        bool play = true;
        if(input.isKeyDown(A) == true)
        {
            data.freq_Hz = 220.0f;
        }
        else if(input.isKeyDown(A_S) == true)
        {
            data.freq_Hz = 233.08f;
        }
        else if(input.isKeyDown(B) == true)
        {
            data.freq_Hz = 246.94f;
        }
        else if(input.isKeyDown(C) == true)
        {
            data.freq_Hz = 130.81f;
        }
        else if(input.isKeyDown(C_S) == true)
        {
            data.freq_Hz = 138.59f;
        }
        else if(input.isKeyDown(D) == true)
        {
            data.freq_Hz = 146.83f;
        }
        else if(input.isKeyDown(D_S) == true)
        {
            data.freq_Hz = 155.56f;
        }
        else if(input.isKeyDown(E) == true)
        {
            data.freq_Hz = 164.81f;
        }
        else if(input.isKeyDown(F) == true)
        {
            data.freq_Hz = 174.61f;
        }
        else if(input.isKeyDown(F_S) == true)
        {
            data.freq_Hz = 185.0f;
        }
        else if(input.isKeyDown(G) == true)
        {
            data.freq_Hz = 196.00f;
        }
        else if(input.isKeyDown(G_S) == true)
        {
            data.freq_Hz = 207.65f;
        }
        else
        {
            play = false;
        }

        if(input.isKeyDown(O0) == true)
        {
            data.freq_Hz /= 8;
        }
        else if(input.isKeyDown(O1) == true)
        {
            data.freq_Hz /= 4;
        }
        else if(input.isKeyDown(O2) == true)
        {
            data.freq_Hz /= 4;
        }
        else if(input.isKeyDown(O3) == true)
        {
            // Do nothing
        }
        else if(input.isKeyDown(O4) == true)
        {
            data.freq_Hz *= 2;
        }
        else if(input.isKeyDown(O5) == true)
        {
            data.freq_Hz *= 4;
        }
        else if(input.isKeyDown(O6) == true)
        {
            data.freq_Hz *= 8;
        }
        else if(input.isKeyDown(O7) == true)
        {
            data.freq_Hz *= 16;
        }
        else if(input.isKeyDown(O8) == true)
        {
            data.freq_Hz *= 32;
        }

        if(play == true)
        {
            error = Pa_StartStream(p_sinStream);
        }
        else
        {
            data.frameTime = 0;
            error = Pa_StopStream(p_sinStream);
        }

    } while(input.wasKeyPressed(EXIT) == false);

    /* Close PortAudio */
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
