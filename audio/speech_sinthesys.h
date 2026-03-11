#pragma once

#include <iostream>
#include <cstring>
#include <math.h>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include "audio.h"

using namespace std;

struct PhonemeInfo {
    int type;  // e.g., SpeechSinthesys::VOWEL, SpeechSinthesys::STOP, ...
    bool voiced;
    int f1, f2, f3;     // 0 if not applicable
};


class SpeechSinthesys {
    static mutex _dictLoadMutex;
    static unordered_map<string, vector<string>> _phonemeLookup;
    static unordered_map<string, PhonemeInfo> phonemeTable;

    enum PhonemeType {
        VOWEL,
        STOP,
        FRICATIVE,
        AFFRICATE,
        NASAL,
        LIQUID,
        SEMIVOWEL,
        ASPIRATE
    };
    double _FC = 120.0; // Fundamental frequency in Hz
    double _BW = 50.0; // Bandwidth for filters, higher = softer, lower = sharper

    // Audio components
    Sawtooth srcWavef; // Base/Source waveform generator
    SumFilters sumF;
    SingleBellFilter bf1, bf2, bf3;
    Envelope mainEnv;
public:
    SpeechSinthesys() : srcWavef(_FC), sumF(&srcWavef), mainEnv(&srcWavef, 0.03, 0.2, 0.8, 0.2), bf1(&sumF, 440, _FC/_BW), bf2(&sumF, 440, _FC/_BW), bf3(&sumF, 440, _FC/_BW) {
        srcWavef.setActive(false);
        bf1.setActive(false);
        bf2.setActive(false);
        bf3.setActive(false);
        sumF.setActive(true);
    }
    static void loadDict(string fileName);

    vector<string> getPhonemes(string word);
    void speakPhonemes(vector<string> phonemes, double crossover);

    void setFC(double fc) { _FC = fc; }
    void setBW(double bw) { _BW = bw; }
};

mutex SpeechSinthesys::_dictLoadMutex;
unordered_map<string, vector<string>> SpeechSinthesys::_phonemeLookup;
unordered_map<string, PhonemeInfo> SpeechSinthesys::phonemeTable = {
    // Vowels
    {"AA", {VOWEL, true, 700, 1100, 2450}},
    {"AE", {VOWEL, true, 660, 1700, 2400}},
    {"AH", {VOWEL, true, 730, 1090, 2440}},
    {"AO", {VOWEL, true, 570, 840, 2410}},
    {"AW", {VOWEL, true, 620, 1190, 2390}},
    {"AY", {VOWEL, true, 660, 1700, 2400}},
    {"EH", {VOWEL, true, 530, 1850, 2500}},
    {"ER", {VOWEL, true, 490, 1350, 1690}},
    {"EY", {VOWEL, true, 530, 1840, 2480}},
    {"IH", {VOWEL, true, 400, 2000, 2550}},
    {"IY", {VOWEL, true, 270, 2290, 3010}},
    {"OW", {VOWEL, true, 430, 820, 2700}},
    {"OY", {VOWEL, true, 500, 700, 2700}},
    {"UH", {VOWEL, true, 440, 1020, 2240}},
    {"UW", {VOWEL, true, 300, 870, 2240}},

    // Stops (voiced / voiceless - implemented as procedural burst+transition)
    {"B", {STOP, true, 0, 0, 0}},
    {"D", {STOP, true, 0, 0, 0}},
    {"G", {STOP, true, 0, 0, 0}},
    {"K", {STOP, false, 0, 0, 0}},
    {"P", {STOP, false, 0, 0, 0}},
    {"T", {STOP, false, 0, 0, 0}},

    // Fricatives - use bandpass filtering of noise around center freq
    {"F", {FRICATIVE, false, 1000, 0, 0}},
    {"V", {FRICATIVE, true, 1000, 0, 0}},
    {"TH", {FRICATIVE, false, 1200, 0, 0}},
    {"DH", {FRICATIVE, true, 1200, 0, 0}},
    {"S", {FRICATIVE, false, 4000, 0, 0}},
    {"Z", {FRICATIVE, true, 4000, 0, 0}},
    {"SH", {FRICATIVE, false, 2500, 0, 0}},
    {"ZH", {FRICATIVE, true, 2500, 0, 0}},

    // Affricates - combo of stop burst + fricative tail
    {"CH", {AFFRICATE, false, 0, 0, 0}},  // T + SH
    {"JH", {AFFRICATE, true, 0, 0, 0}},   // D + ZH

    // Nasals - damped vowels, with low formants
    {"M", {NASAL, true, 250, 1100, 2100}},
    {"N", {NASAL, true, 300, 1300, 2300}},
    {"NG", {NASAL, true, 300, 2000, 2700}},

    // Liquids - voiced, vowel-like with distinct formants
    {"L", {LIQUID, true, 400, 1300, 2400}},
    {"R", {LIQUID, true, 300, 1100, 1600}},  // Lower F3

    // Semivowels - short vowels
    {"W", {SEMIVOWEL, true, 300, 870, 2240}},  // like UW
    {"Y", {SEMIVOWEL, true, 270, 2290, 3010}}, // like IY

    // Aspirate - breathy noise
    {"HH", {ASPIRATE, false, 1500, 0, 0}}  // center around 1500 Hz
};



#include "speech_sinthesys.cpp"