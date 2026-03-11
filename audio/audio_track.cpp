#include "audio.h"

AudioTrack::AudioTrack(string fileName) {
    len = 0;
    play_pos = 0;
    volume = MAX_VOLUME;
    audio_pan = 0;
    pan_mult_l = MAX_VOLUME;
    pan_mult_r = MAX_VOLUME;
    buffer = nullptr;

    playing = false;
    loop = false;
    inited = false;

    /* Load the WAV */
    // the specs, length and buffer of our wav are getting filled
    if (SDL_LoadWAV(fileName.c_str(), &spec, &buffer, &len) == NULL)
        return;

    SDL_AudioCVT cvt;

    memset(&cvt, 0, sizeof(cvt));
    int ret = SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq, AudioLib::spec.format, AudioLib::spec.channels, AudioLib::spec.freq);
    if (ret < 0) {
        cout << "Error loading conversion for audio data\n";

    } else if (ret == 1) {
        cvt.buf = (Uint8 *)malloc(len * cvt.len_mult);
        cvt.len = len;
        memcpy(cvt.buf, buffer, len);

        if (SDL_ConvertAudio(&cvt) != 0) {
            cout << "Error converting audio\n";
        } else {
            cout << "Conversion of audio data completed successfuly\n";
            SDL_FreeWAV(buffer);
            buffer = (Uint8 *)malloc(len * cvt.len_mult);
            memcpy(buffer, cvt.buf, cvt.len_cvt);
            len = cvt.len_cvt;
        }

        free(cvt.buf);

        spec = AudioLib::spec;
    }

    if (ret >= 0) { // no conversion needed=0 / converted=1
        inited = true;
        audioInterfaceGlobal.addTrack(this);
    }
}

// position control

void AudioTrack::rollToBeginning() {
    if (!inited) return;
    play_pos = 0;
}
/// @brief sets the audio's progress
void AudioTrack::rollTo(double seconds) {
    if (!inited) return;
    if (seconds < 0)
        seconds = 0;
    uint32_t tmp_play = seconds * spec.freq * spec.channels * ((spec.format & SDL_AUDIO_MASK_BITSIZE) >> 3); // this big ending piece is # of B per sample
    if (tmp_play <= len)
        play_pos = tmp_play; // set if valid
    else
        play_pos = 0; // to 0 if over
}

// setters

void AudioTrack::setPlaying(bool st) {
    if (!inited) return;
    playing = st;
}
void AudioTrack::setLooping(bool st) {
    if (!inited) return;
    loop = st;
}
void AudioTrack::setVolume(double newVolume) {
    if (!inited) return;
    clamp(newVolume, 0., 10.);
    volume = newVolume * MAX_VOLUME;
}
void AudioTrack::setPan(double newPan) {
    if (!inited) return;
    clamp(newPan, -1., 1.);
    audio_pan = newPan;

    calculate_pan();
}
void AudioTrack::calculate_pan() { // -1 left, 0 center, 1 right
    double it = (audio_pan + 1.) / 2.;
    pan_mult_l = MAX_VOLUME * (1. - it);
    pan_mult_r = MAX_VOLUME * (it);
}

// getters

bool AudioTrack::getPlayingStatus() {
    return playing;
}
bool AudioTrack::getLooping() {
    return loop;
}
double AudioTrack::getLength() {
    if (!inited) return -1;
    uint32_t samples = (len / spec.channels) / ((spec.format & SDL_AUDIO_MASK_BITSIZE) >> 3); // # of B per sample
    double out = samples / (double)spec.freq;
    return out;
}
double AudioTrack::getVolume() {
    if (!inited) return -1;
    return volume / (double)MAX_VOLUME;
}
double AudioTrack::getPan() {
    return audio_pan;
}

AudioTrack AudioTrack::operator=(AudioTrack a) {
    len = a.len;
    play_pos = a.play_pos;
    spec = a.spec;
    playing = a.playing;
    loop = a.loop;
    inited = a.inited;

    buffer = (uint8_t *)malloc(a.len);
    memcpy(buffer, a.buffer, len);

    return *this;
}
AudioTrack AudioTrack::operator()(AudioTrack a) {
    return (*this = a);
}

AudioTrack::~AudioTrack() {
    if (inited) {
        audioInterfaceGlobal.removeTrack(this);
    }
    free(buffer);
}