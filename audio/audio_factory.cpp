#include "audio.h"

AudioFactory::AudioFactory() {
    audioInterfaceGlobal.addFactory(this);
}
AudioFactory::~AudioFactory() {
    audioInterfaceGlobal.removeFactory(this);
}

void AudioFactory::setActive(bool a) {
    active = a;
}
void AudioFactory::setVolume(double v) {
    clamp(v, 0., 1.);
    volume = v;
}

bool AudioFactory::isActive() {
    return active;
}
double AudioFactory::getVolume() {
    return volume;
}

void AudioFactory::addFilter(Filter *filter) {
    filters.push_back(filter);
}
void AudioFactory::removeFilter(Filter *filter) {
    for (auto it = filters.begin(); it != filters.end(); ++it) {
        if (*it == filter) {
            filters.erase(it);
            break;
        }
    }
}
void AudioFactory::generate_internal(sample_t *buffer, int len) {
    generate(buffer, len); // generate audio data
    
    // Apply filters
    for (auto &filter : filters)
        if (filter->isActive())
            filter->process(buffer, len);
}