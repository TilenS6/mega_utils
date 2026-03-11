#include "audio.h"

template<class T>
SumFilters::SumFilters(T *element) : Filter(element) {
}

SumFilters::~SumFilters() {
    if (tmp) free(tmp);
    if (sum) free(sum);
}

void SumFilters::process(sample_t *buffer, int len) {    
    int aplied_filters = 0;
    
    if (!tmp) tmp = (sample_t *)malloc(len * BYTES_PER_SAMPLE);
    if (!sum) sum = (suming_t *)malloc(len * sizeof(suming_t));
    memset(sum, 0, len * sizeof(suming_t));

    // Apply filters
    for (auto &filter : filters) {
        if (filter->isActive()) {
            memcpy(tmp, buffer, len * BYTES_PER_SAMPLE); // copy buffer to tmp
            filter->process(tmp, len);
            for (int i = 0; i < len; i++) {
                sum[i] += tmp[i]; // sum the filtered data
            }
            aplied_filters++;
        }
    }
    for (int i = 0; i < len; i++) {
        clamp(sum[i], SAMPLE_MIN, SAMPLE_MAX); // clamp the sum to prevent overflow
        tmp[i] = sum[i];
    }
    if (aplied_filters > 0) {
        memcpy(buffer, tmp, len * BYTES_PER_SAMPLE); // copy the summed data back to buffer
    }
}