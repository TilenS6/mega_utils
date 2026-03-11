#include "audio.h"

AudioInterface::AudioInterface() {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec want;
    memset(&want, 0, sizeof(want));

    want.freq = DESIRED_OUTPUT_FREQ;
#ifdef HIGHER_BIT_DEPTH
    want.format = AUDIO_S32; //! IMPORTANT, ref: 7419, here is the main thing that can change
#else
    want.format = AUDIO_S16; //! IMPORTANT, ref: 7419, here is the main thing that can change
#endif
    want.channels = 2;
    want.samples = DESIRED_OUTPUT_SAMPLES;
    want.callback = audio_callback;
    want.userdata = NULL;

    memset(&AudioLib::spec, 0, sizeof(AudioLib::spec));
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &AudioLib::spec, SDL_AUDIO_ALLOW_SAMPLES_CHANGE);

    if (dev == 0) {
        cout << "Error opening audio device!\n";
        return;
    }

    /* Start playing */
    SDL_PauseAudioDevice(dev, false);
    inited = true;
}

void AudioInterface::closeAudio() {
    if (inited) {
        SDL_PauseAudioDevice(dev, true);
        SDL_CloseAudio();
        inited = false;
    }
}

AudioInterface::~AudioInterface() {
    if (!inited) return; // if not inited, do nothing
    SDL_PauseAudioDevice(dev, true);
    SDL_CloseAudio();
    inited = false;
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
bool val = false;
void AudioInterface::audio_callback(void *userdata, Uint8 *stream, int len) {
    memset(stream, 0, len);
    for (int i = 0; i < audioInterfaceGlobal.tracks.size(); ++i) {
        if (!audioInterfaceGlobal.tracks[i]->playing) continue; // if not playing

        if (audioInterfaceGlobal.tracks[i]->play_pos >= audioInterfaceGlobal.tracks[i]->len) { // if play_pos is too far
            audioInterfaceGlobal.tracks[i]->play_pos = 0;                                      // beggining
            audioInterfaceGlobal.tracks[i]->playing = audioInterfaceGlobal.tracks[i]->loop;    // pause or not pause if looping
            continue;
        }

        // setting copy_len to fit the track at the end
        int copy_len = len;
        if (audioInterfaceGlobal.tracks[i]->play_pos + len > audioInterfaceGlobal.tracks[i]->len) {
            copy_len = audioInterfaceGlobal.tracks[i]->len - audioInterfaceGlobal.tracks[i]->play_pos; // what is left
        }

        //! IMPORTANT, ref: 7419
        sample_t *buff_play_pos = (sample_t *)(audioInterfaceGlobal.tracks[i]->buffer + audioInterfaceGlobal.tracks[i]->play_pos);
        suming_t vol = (suming_t)audioInterfaceGlobal.tracks[i]->volume;
        suming_t pan_mult[2] = {(suming_t)audioInterfaceGlobal.tracks[i]->pan_mult_l, (suming_t)audioInterfaceGlobal.tracks[i]->pan_mult_r};
        for (int j = 0; j < copy_len * LEN_MULT_TO_SAMPLES; ++j) {
            suming_t new_val = ((*(buff_play_pos + j)) * vol * pan_mult[j & 0x1]) >> (BYTES_PER_SAMPLE * 8 * 2 - 1); // from data, *8 to shift whole B, *2 we have two multiplications of 16b vales
            new_val += *((sample_t *)(stream) + j);                                                               // from stream
            clamp(new_val, SAMPLE_MIN, SAMPLE_MAX);                                                                // clipping
            *((sample_t *)(stream) + j) = (sample_t)new_val;                                                       // copy
        }
        // memcpy(stream, audioInterfaceGlobal.tracks[i]->buffer + audioInterfaceGlobal.tracks[i]->play_pos, copy_len); // copy

        if (len == copy_len) {
            audioInterfaceGlobal.tracks[i]->play_pos += copy_len; // data increment if not the end of the data
        } else {
            audioInterfaceGlobal.tracks[i]->play_pos = 0; // revert
            audioInterfaceGlobal.tracks[i]->playing = audioInterfaceGlobal.tracks[i]->loop;
        }
    }

    // ======== FACTORIES ========
    for (int i = 0; i < audioInterfaceGlobal.factories.size(); ++i) {
        if (audioInterfaceGlobal.factories[i]->isActive()) {
            // Generate audio data
            sample_t *buff = (sample_t *)malloc(BYTES_PER_SAMPLE * len * 2); // *2 for stereo
            audioInterfaceGlobal.factories[i]->generate_internal(buff, len * LEN_MULT_TO_SAMPLES);
            suming_t volume = audioInterfaceGlobal.factories[i]->getVolume() * MAX_VOLUME; // volume from 0 to 1, *MAX_VOLUME to get the real value

            // Mix generated audio data with the stream
            for (int j = 0; j<len * LEN_MULT_TO_SAMPLES; ++j) {
                suming_t new_val = (buff[j] * volume) >> (BYTES_PER_SAMPLE * 8 - 1); // from data, *8 to shift whole B, -1 signed
                new_val += *((sample_t *)(stream) + j); // from stream
                clamp(new_val, SAMPLE_MIN, SAMPLE_MAX); // clipping
                *((sample_t *)(stream) + j) = (sample_t)new_val; // copy
            }
        }
    }

    // ======== GLOBAL FILTERS ========
    for (int i = 0; i < audioInterfaceGlobal.filters.size(); ++i) {
        if (audioInterfaceGlobal.filters[i]->isActive()) {
            audioInterfaceGlobal.filters[i]->process((sample_t *)stream, len * LEN_MULT_TO_SAMPLES);
        }
    }

    globalAudioDebug.pushBuffer((sample_t *)stream, len * LEN_MULT_TO_SAMPLES); // Visualize audio data
}









 void AudioInterface::addTrack(AudioTrack *track) {
    tracks.push_back(track);
}
void AudioInterface::addFactory(AudioFactory *factory) {
    factories.push_back(factory);
}
void AudioInterface::addFilter(Filter *filter) {
    filters.push_back(filter);
}

void AudioInterface::removeTrack(AudioTrack *track) {
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
        if (*it == track) {
            tracks.erase(it);
            break;
        }
    }
}
void AudioInterface::removeFactory(AudioFactory *factory) {
    for (auto it = factories.begin(); it != factories.end(); ++it) {
        if (*it == factory) {
            factories.erase(it);
            break;
        }
    }
}
void AudioInterface::removeFilter(Filter *filter) {
    for (auto it = filters.begin(); it != filters.end(); ++it) {
        if (*it == filter) {
            filters.erase(it);
            break;
        }
    }
}