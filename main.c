#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define DEFAULT_FREQ 44100
#define DEFAULT_CHANNELS 2
#define AUDIOSPEC_FORMAT AUDIO_S16LSB
#define BUFFER_SIZE 2048 

void audio_callback(void* userdata, Uint8* stream, int len) {
    FILE* file = (FILE*)userdata;
    static Uint8 buffer[BUFFER_SIZE];
    static int buffer_index = 0;
    static int buffer_length = 0;
    static long marker_position = 0;

    int remaining_length = len;
    while (remaining_length > 0) {
        if (buffer_index >= buffer_length) {
            marker_position = ftell(file);
            buffer_length = fread(buffer, 1, BUFFER_SIZE, file);
            if (buffer_length == 0) {
                if (feof(file)) {
                    fprintf(stderr, "EOF reached.\n");
					fflush(stderr);
                }
                if (ferror(file)) {
                    fprintf(stderr, "Error reading from file.\n");
					fflush(stderr);
                }
                clearerr(file); 
                fseek(file, marker_position, SEEK_SET);
                SDL_Delay(500); 
                continue;
            }
            buffer_index = 0;
        }

        int bytes_to_copy = (buffer_length - buffer_index < remaining_length)
                                ? buffer_length - buffer_index
                                : remaining_length;
        memcpy(stream, &buffer[buffer_index], bytes_to_copy);
        stream += bytes_to_copy;
        buffer_index += bytes_to_copy;
        remaining_length -= bytes_to_copy;
    }
}

int main(int argc, char* argv[]) {
    SDL_AudioSpec spec;
    int channels = DEFAULT_CHANNELS;
    int freq = DEFAULT_FREQ;
    int c;

    // Redirect stdout and stderr to /dev/null
    stdout = freopen("/tmp/audiobridgelog.log", "w", stdout);
	stderr = fdopen(dup(fileno(stdout)), "w");

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
				fflush(stderr);
                return 1;
        }
    }

    spec.freq = freq;
    spec.channels = channels;
    spec.format = AUDIOSPEC_FORMAT;
    spec.samples = BUFFER_SIZE;
    spec.callback = audio_callback;

    FILE* file = fdopen(0, "rb"); 

    if (file == NULL) {
        fprintf(stderr, "Unable to open input stream!\n");
        return 1;
    }

    spec.userdata = file;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		fflush(stderr);
        return 1;
    }

    if (SDL_OpenAudio(&spec, NULL) < 0) {
    fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
	fflush(stderr);
    return 1;
}
	fprintf(stderr, "Waiting for audio...!\n");
	fflush(stderr);
	SDL_Delay(1000);  // wait for some audio to accumulate in stream before playing

	SDL_PauseAudio(0);  // start

    while (1) {
        SDL_Delay(100);
    }

    SDL_CloseAudio();
    fclose(file);

    fflush(stdout);
    fflush(stderr);

    return 0;
}