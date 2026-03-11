#include "audio.h"

template<class T>
Filter::Filter(T *element) {
    if (element != nullptr) {
        element->addFilter(this);
        removeFunc = [element, this]() {
            element->removeFilter(this);
        };

    } else {
        audioInterfaceGlobal.addFilter(this);
        removeFunc = [this]() {
            audioInterfaceGlobal.removeFilter(this);
        };
    }
}

Filter::~Filter() {
    if (removeFunc) removeFunc();
}

void Filter::setActive(bool a) {
    active = a;
}

bool Filter::isActive() {
    return active;
}