/*
! This audio library should be automatic!
Works with SDL2, Camera class and other mega_utils components

* automatically mix all created AudioTracks and AudioFactories,
* apply all created Filters and output the final audio to the audio device,
* allow real-time control of all these elements (position, volume, pan for tracks, parameters for filters, etc.)
* allow dynamic creation and deletion of all these elements.



Example in main:

Camera c(WIDTH, HEIGHT, "Speaking bot");
globalAudioDebug.enableDebug(c.r);

double Fc = 478, bw = 50; // +bw: softer, -bw: sharper
Sawtooth saw(Fc);
saw.setActive(true);

SumFilters sum(&saw);

SingleBellFilter sawAA1(&sum, 440, Fc/bw);
SingleBellFilter sawAA2(&sum, 1020, Fc/bw);
SingleBellFilter sawAA3(&sum, 2240, Fc/bw);

Envelope env(&saw, 0.03, 0.2, 0.8, 0.2);


SpeechSinthesys ss;
ss.loadDict("cmudict.dict");
vector<string> phonemes = ss.getPhonemes("hello");
for (auto p : phonemes) cout << p << " ";
cout << endl;
system("sleep 10");
*/

#pragma once
#include <iostream>
#include <cstring>
#include <math.h>
#include <vector>
#include <SDL2/SDL.h>
#include <functional>

using namespace std;

#define DESIRED_OUTPUT_FREQ 96000
#define DESIRED_OUTPUT_SAMPLES 512

// from 16b to 32b audio depth
#define HIGHER_BIT_DEPTH


#ifdef HIGHER_BIT_DEPTH
typedef int32_t sample_t;
typedef __int128_t suming_t;
#define SAMPLE_MAX INT32_MAX
#define SAMPLE_MIN INT32_MIN
#else
typedef int16_t sample_t;
typedef int64_t suming_t;
#define SAMPLE_MAX INT16_MAX
#define SAMPLE_MIN INT16_MIN

// #define sample_t uint16_t
// #define suming_t uint64_t
#endif
#define BYTES_PER_SAMPLE sizeof(sample_t)

#define LEN_MULT_TO_SAMPLES (1. / BYTES_PER_SAMPLE) //! IMPORTANT, ref: 7419
#define MAX_VOLUME (sample_t)(pow(2, BYTES_PER_SAMPLE*8 - 1) - 1) // *8 -1 because of signed values
#define clamp(val, lo, hi) val = ((val < lo) ? lo : ((val > hi) ? hi : val))

class AudioInterface;
class AudioTrack;
class AudioFactory;
class Filter;

namespace AudioLib {
    SDL_AudioSpec spec;
}

class AudioInterface {
    SDL_AudioDeviceID dev;

    vector<AudioTrack *> tracks;
    vector<AudioFactory *> factories;
    vector<Filter *> filters;
    static void audio_callback(void *userdata, Uint8 *stream, int len);

    bool inited = false;

public:
    AudioInterface();
    ~AudioInterface();
    void closeAudio();

    void addTrack(AudioTrack *track);
    void addFactory(AudioFactory *factory);
    void addFilter(Filter *filter);

    void removeTrack(AudioTrack *track);
    void removeFactory(AudioFactory *factory);
    void removeFilter(Filter *filter);
} audioInterfaceGlobal;

// AudioTrack is a class that represents an audio file track
//! This should be converted to derived AudioFactory class
class AudioTrack {
    uint32_t len, play_pos;          // length in B of this audio
    uint64_t volume;                 //! IMPORTANT, ref: 7419 (volume, allows boost)
    double audio_pan;                // -1 left, 0 center, 1 right
    SDL_AudioSpec spec;              // the specs of our piece of audio
    uint32_t pan_mult_l, pan_mult_r; //! IMPORTANT, ref: 7419
    uint8_t *buffer;                 // audio data buffer
    bool inited;

    bool playing, loop;

    inline void calculate_pan();

public:
    AudioTrack(string);

    // position control
    void rollToBeginning();
    void rollTo(double);

    // setters
    void setPlaying(bool);
    void setLooping(bool);
    void setVolume(double);
    void setPan(double);

    // getters
    bool getPlayingStatus();
    bool getLooping();
    double getLength();
    double getVolume();
    double getPan();

    // copy
    AudioTrack operator=(AudioTrack);
    AudioTrack operator()(AudioTrack);

    // destructors
    ~AudioTrack();

    friend class AudioInterface;
};

// Filter is a virtual class that processes audio data in real time
class Filter {
    std::function<void()> removeFunc; // function to call in destructor

protected:
    bool active = true;

public:
    template<class T>
    Filter(T *element = nullptr);
    virtual ~Filter();

    virtual void process(sample_t *buffer, int len) = 0;

    void setActive(bool a);
    bool isActive();
};

// AudioFactory is a virtual class that generates audio data on the fly
class AudioFactory {
protected:
    bool active = false;
    double volume = .5; // 0-1 range, 1 is max volume
    vector<Filter *> filters;

public:
    AudioFactory();
    virtual ~AudioFactory();

    // You implement this and nothing more (generate max volume data):
    virtual void generate(sample_t *buffer, int len) = 0;

    void setActive(bool a);
    bool isActive();

    void setVolume(double v);
    double getVolume();

    void addFilter(Filter *filter);
    void removeFilter(Filter *filter);
    void generate_internal(sample_t *buffer, int len);
};

class Envelope : public Filter {
    // Visual: https://community.jmp.com/t5/image/serverpage/image-id/25719iB5C92BE01DEAF001?v=v2
    double _attack, _decay, _sustain, _release;
    bool _before; // Key pressed before? Used to detect key press events

    double _sustainDuration; // Sustain duration in seconds, used for manualRun. If set to <0 it will hold sustain forever
    bool _override = false; // If true, it will override the current envelope phase and start from attack phase
    double _ORattack, _ORdecay, _ORrelease; // Overriden parameters for manualRun

    double _progress;
    int _phase; // 0: attack, 1: decay, 2: sustain, 3: release
public:
    Envelope(AudioFactory *element, double attack, double decay, double sustain, double release); // A,D,R in seconds, S in volume multiplier. Attack: TIME from key pressed to max volume, Decay: TIME from max volume to sustain, Sustain: hold VOLUME, Release: TIME from sustain to 0 when key released

    void process(sample_t *buffer, int len) override;

    void setKey(bool pressed);
    void manualRun(double duration); // Time in seconds, measured from start to end of envelope. If duration is less than attack + decay + release, it will shorten A, D and R parameters proportionally, sustain phase will be deactivated.

    void setParams(double attack, double decay, double sustain, double release); // A[seconds], D[seconds], S[volume: 0-1], R[seconds]
    void setAttack(double a); // Set attack time in seconds
    void setDecay(double d); // Set decay time in seconds
    void setSustain(double s); // Set sustain volume (0-1)
    void setRelease(double r); // Set release time in seconds

    double getAttack();
    double getDecay();
    double getSustain();
    double getRelease();
};

class SumFilters : public Filter {
    sample_t *tmp = nullptr; // Temporary buffer for processing
    suming_t *sum = nullptr; // Buffer for summing all filters
    int tmpLen = 0; // Length of the temporary buffer

    vector<Filter *> filters; // List of filters to sum
public:
    template<class T>
    SumFilters(T *element = nullptr);
    ~SumFilters();

    void addFilter(Filter *f) { filters.push_back(f); }
    void removeFilter(Filter *f) {
        for (auto it = filters.begin(); it != filters.end(); ++it) {
            if (*it == f) {
                filters.erase(it);
                break;
            }
        }
    }

    void process(sample_t *buffer, int len) override;
};

class AudioDebug {
    SDL_Renderer *r = nullptr; // Camera for visualizing audio data
    sample_t *lastBuffer = nullptr; // Last buffer visualized
    int lastLen = 0; // Length of the last buffer visualized
public:
    void enableDebug(SDL_Renderer *_r) { r = _r; }
    void pushBuffer(sample_t *buffer, int len);
    void visualizeSDL();
} globalAudioDebug;

#include "audio_interface.cpp"
#include "audio_track.cpp"
#include "audio_factory.cpp"
#include "audio_filter.cpp"
#include "audio_envelope.cpp"
#include "audio_sumfilters.cpp"
#include "audio_debug.cpp"

#include "demo_factory.cpp"
#include "demo_filter.cpp"