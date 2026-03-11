#include "audio.h"

class SingleBellFilter : public Filter {
    double Fc, Fs, Q; // center frequency, sample rate, quality factor
    double b0, b1, b2; // feedforward coefficients
    double a1, a2; // feedback coefficients

    double x1[2] = {0, 0}, x2[2] = {0, 0}; // left and right channel
    double y1[2] = {0, 0}, y2[2] = {0, 0};
public:
    template<class T>
    SingleBellFilter(T *element, double centerFreq, double qualityFactor) : Filter(element) {
        Fs = AudioLib::spec.freq; // sample rate from audio interface
        changeParams(centerFreq, qualityFactor);
    }

    void process(sample_t *buffer, int len) override {
        for (int i = 0; i < len; i++) {
            uint8_t channel = i&0x1;
            double x = (double)buffer[i] / SAMPLE_MAX;  // normalize to [-1,1]
            double y = b0*x + b1*x1[channel] + b2*x2[channel] - a1*y1[channel] - a2*y2[channel];
            suming_t yy = y * SAMPLE_MAX; // scale back to original range
            buffer[i] = clamp(yy, SAMPLE_MIN, SAMPLE_MAX);
            x2[channel] = x1[channel]; x1[channel] = x;
            y2[channel] = y1[channel]; y1[channel] = y;
        }
    }

    double getCenterFrequency() {
        return Fc;
    }
    double getQualityFactor() {
        return Q;
    }

    void setCenterFrequency(double centerFreq) {
        changeParams(centerFreq, Q);
    }
    void setQualityFactor(double qualityFactor) {
        changeParams(Fc, qualityFactor);
    }
    void changeParams(double centerFreq, double qualityFactor) {
        reset();
        Fc = centerFreq; 
        Q = qualityFactor;
        
        // Recalculate coefficients
        double omega = 2 * M_PI * Fc / Fs;
        double alpha = sin(omega) / (2 * Q);

        b0 = alpha;
        b1 = 0;
        b2 = -alpha;
        double a0 = 1 + alpha;
        a1 = -2 * cos(omega);
        a2 = 1 - alpha;

        // normalize:
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
    }
    void reset() {
        x1[0] = x1[1] = x2[0] = x2[1] = 0.0;
        y1[0] = y1[1] = y2[0] = y2[1] = 0.0;
    }
};