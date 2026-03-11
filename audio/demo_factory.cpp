#include "audio.h"

class SineWave : public AudioFactory {
    unsigned long c = 0;
public:
    double hz;

    SineWave(double _hz = 440) { hz = _hz; }
    void generate(sample_t *buffer, int len) override {
        for (int i = 0; i < len; i+=2) {
            buffer[i] = buffer[i+1] = sin(c * (hz / AudioLib::spec.freq) * 2 * M_PI) * ((sample_t)SAMPLE_MAX);
            ++c;
        }
    }
};

class Noise : public AudioFactory {
public:
    void generate(sample_t *buffer, int len) override {
        for (int i = 0; i < len; i+=2) {
            buffer[i] = buffer[i+1] = (int32_t)rand() - rand();
        }
    }
};

class Sawtooth : public AudioFactory {
    unsigned long c = 0;
    double _hz, _increment, _phase = 0.0;
public:
    Sawtooth(double hz = 440) { changeHz(hz); }
    void changeHz(double newHz) {
        _hz = newHz;
        _increment = (SAMPLE_MAX * _hz) / AudioLib::spec.freq; // 255 for 8-bit resolution
    }
    void generate(sample_t *buffer, int len) override {
        for (int i = 0; i < len; i+=2) {
            buffer[i] = buffer[i+1] = (sample_t)(_phase);
            _phase += _increment;
            if (_phase >= SAMPLE_MAX)
                _phase = SAMPLE_MIN + (_phase-SAMPLE_MAX); // wrap around
        }
    }
};