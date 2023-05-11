#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define DEFAULT_FREQ 44100
#define DEFAULT_CHANNELS 2
#define AUDIOSPEC_FORMAT AUDIO_S16LSB

void audio_callback(void *userdata, Uint8 *stream, int len) {
    FILE *file = (FILE*) userdata;
    while (fread(stream, len, 1, file) != 1) {
        clearerr(file);  // clear EOF condition
        SDL_Delay(500);  // wait for more data to become available
    }
}

int main(int argc, char* argv[]) {
    SDL_AudioSpec spec;
    int channels = DEFAULT_CHANNELS;
    int freq = DEFAULT_FREQ;
    int c;

    // Redirect stdout and stderr to /dev/null
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    while ((c = getopt(argc, argv, "c:r:")) != -1) {
        switch (c) {
            case 'c':
                channels = atoi(optarg);
                break;
            case 'r':
                freq = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid argument -%c\n", c);
                return 1;
        }
    }

    spec.freq = freq;
    spec.channels = channels;
    spec.format = AUDIOSPEC_FORMAT;
    spec.samples = 2048;
    spec.callback = audio_callback;

    FILE *file = fdopen(0, "rb");  // read from stdin

    if (file == NULL) {
        fprintf(stderr, "Unable to open input stream!\n");
        return 1;
    }

    spec.userdata = file;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_OpenAudio(&spec, NULL) < 0) {
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
        return 1;
    }

    SDL_PauseAudio(0); // Start audio playback

    while (1) {
        SDL_Delay(100);  // Let the audio callback work
    }

    SDL_CloseAudio();
    fclose(file);

    // Flush stdout and stderr at the end
    fflush(stdout);
    fflush(stderr);

    return 0;
}