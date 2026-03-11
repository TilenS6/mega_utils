#include "audio.h"

Envelope::Envelope(AudioFactory *element, double attack, double decay, double sustain, double release) : Filter(element) {
    _attack = attack;
    _decay = decay;
    _sustain = sustain;
    _release = release;

    _phase = -1; // Enevlope is initially inactive
    _progress = 0.0; // Initialize progress
    _before = false;

    _sustainDuration = -1; // Default to infinite sustain
}

void Envelope::process(sample_t *buffer, int len) {
    for (int i = 0; i < len; i += 2) {
        double value = 0.0;

        switch (_phase) {
            case 0: // Attack phase
                _progress += 1. / (_attack * AudioLib::spec.freq);
                if (_progress >= 1) {
                    _progress = 0;
                    _phase = 1; // Move to decay phase
                }
                value = _progress; // Volume increases from 0 to 1
                break;

            case 1: // Decay phase
                _progress += 1 / (_decay * AudioLib::spec.freq);
                if (_progress >= 1) {
                    _progress = 0;
                    _phase = 2; // Move to sustain phase
                }
                value = 1-_progress+_progress*_sustain; // Volume decreases to sustain level
                break;

            case 2: // Sustain phase
                value = _sustain; // Hold sustain level
                break;

            case 3: // Release phase
                _progress += 1 / (_release * AudioLib::spec.freq);
                if (_progress >= 1) {
                    _progress = 0.0;
                    _phase = -1; // Envelope finished
                }
                value = (1-_progress)*_sustain; // Volume decreases to 0
                break;

            default:
                value = 0.; // No sound when envelope is finished
        }

        buffer[i] *= value; // Apply envelope to the audio buffer
        buffer[i+1] *= value; // Apply envelope to the audio buffer
    }
}

void Envelope::manualRun(double duration) { // TODO: test + make override and _sustainDuration work in the upper method
    _phase = 0; // Set phase to attack
    _progress = 0;
    _sustainDuration = duration - _attack - _decay - _release;
    if (_sustainDuration < 0) {
        // If duration is less than total envelope time, proportionally shorten A, D, R
        double totalTime = _attack + _decay + _release;
        double scale = duration / totalTime;
        _ORattack *= scale;
        _ORdecay *= scale;
        _ORrelease *= scale;

        _sustain = 1.0; // Sustain at max volume
        _sustainDuration = 0; // Disable sustain phase
        _override = true; // Indicate that we are overriding the envelope
    } else {
        _override = false;
    }
}

void Envelope::setKey(bool pressed) {
    if (pressed && !_before) { // Key pressed, start attack phase
        _phase = 0; // Set phase to attack
        _progress = 0;
    } else if (!pressed && _before) { // Key released, start release phase
        _phase = 3; // Set phase to release
        _progress = 0; // Reset progress for release phase
    }
    _before = pressed;
}


void Envelope::setParams(double attack, double decay, double sustain, double release) {
    setAttack(attack);
    setDecay(decay);
    setSustain(sustain);
    setRelease(release);
}
void Envelope::setAttack(double a) {
    _attack = a;
}
void Envelope::setDecay(double d) {
    _decay = d;
}
void Envelope::setSustain(double s) {
    _sustain = s;
}
void Envelope::setRelease(double r) {
    _release = r;
}

double Envelope::getAttack() {
    return _attack;
}
double Envelope::getDecay() {
    return _decay;
}
double Envelope::getSustain() {
    return _sustain;
}
double Envelope::getRelease() {
    return _release;
}