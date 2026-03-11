#include "speech_sinthesys.h"

void SpeechSinthesys::loadDict(string fileName) {
    lock_guard<mutex> lock(_dictLoadMutex);

    if (_phonemeLookup.size() > 0) {
        cout << "Dictionary already loaded, skipping." << endl;
        return;
    }
    cout << "Loading dictionary from: " << fileName << endl;

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening dictionary file: " << fileName << endl;
        return;
    }
    _phonemeLookup.clear(); 
    _phonemeLookup.reserve(150000); // enough for cmudict.dict from github

    string line, word, tmp;
    bool firstWord = true;
    while (getline(file, line)) {
        for (int i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == '#') break; // Ignore comments

            if (c == ' ') {
                if (!tmp.empty()) {
                    if (firstWord) {
                        word = tmp;
                        _phonemeLookup.insert({word, vector<string>()});
                        tmp.clear();
                        firstWord = false;
                    } else {
                        _phonemeLookup[word].push_back(tmp);
                        tmp.clear();
                    }
                }
            } else {
                tmp += c;
            }
            
        }
        if (!tmp.empty()) {
            _phonemeLookup[word].push_back(tmp);
            tmp.clear();
            firstWord = true; // Reset for next line
        }
    }
}

vector<string> SpeechSinthesys::getPhonemes(string word) {
    return _phonemeLookup[word];
}


void SpeechSinthesys::speakPhonemes(vector<string> phonemes, double crossover) {
    for (string p : phonemes) {
        p.erase(remove_if(p.begin(), p.end(), ::isdigit), p.end()); // Remove variants like "AA1", "AA2"
        auto it = phonemeTable.find(p);
        if (it == phonemeTable.end()) {
            cerr << "WARN: Phoneme \"" << p << "\" not supported!" << endl;
            continue;
        }
        PhonemeInfo info = it->second;

        switch (info.type) {
            case VOWEL:
                // Synthesize vowel
                bf1.setCenterFrequency(info.f1);
                bf1.setQualityFactor(_FC / _BW);
                bf2.setCenterFrequency(info.f2);
                bf2.setQualityFactor(_FC / _BW);
                bf3.setCenterFrequency(info.f3);
                bf3.setQualityFactor(_FC / _BW);

                bf1.setActive(true);
                bf2.setActive(true);
                bf3.setActive(true);

                mainEnv.setKey(true); // Start the envelope
                break;
            default:
                cout << "WARN: Unsupported phoneme type for synthesis: " << p << endl;
                break;
        }
    }
}