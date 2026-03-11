#include "audio.h"

void AudioDebug::pushBuffer(sample_t *buffer, int len) {
    if (!buffer || len <= 0) return; // Check if renderer and buffer are valid
    if (!lastBuffer || lastLen != len) {
        if (lastBuffer) free(lastBuffer); // Free previous buffer
        lastBuffer = (sample_t *)malloc(len*sizeof(sample_t)); // Allocate new buffer
        lastLen = len;
    }
    memcpy(lastBuffer, buffer, len*sizeof(sample_t)); // Copy new buffer data
}
void AudioDebug::visualizeSDL() {
    if (!r || !lastBuffer || lastLen <= 0) return; // Check if renderer and buffer are valid
    int w, h;
    SDL_GetRendererOutputSize(r, &w, &h);

    SDL_SetRenderDrawColor(r, 100, 100, 100, 255); // Set color to red for left channel
    SDL_RenderDrawLine(r, 0, h/4, w, h/4); // Draw horizontal line at 1/4 height
    SDL_RenderDrawLine(r, 0, 3*h/4, w, 3*h/4); // Draw horizontal line at 3/4 height

    int lastX = 0;
    static int lastYleft = h/2, lastYRight = h/2;
    double normFactor = 1.0 / ((double)SAMPLE_MAX - (double)SAMPLE_MIN);
    for (int i = 0; i < lastLen-1; i += 2*16) {
        if (lastBuffer[i] > SAMPLE_MAX) cout << "Watahell? " << lastBuffer[i] << endl;

        double left  = ((double)lastBuffer[i] - SAMPLE_MIN)*normFactor; // Normalize to 0-1 range
        double right = ((double)lastBuffer[i + 1] - SAMPLE_MIN)*normFactor;

        int x = (w*i)/(double)lastLen; // Scale to screen width
        int yLeft = (1-left)*(h/2);
        int yRight = (1-right)*(h/2) + h/2;

        SDL_SetRenderDrawColor(r, 255, 100, 100, 255); // Red for left channel
        SDL_RenderDrawLine(r, lastX, lastYleft, x, yLeft);
        SDL_SetRenderDrawColor(r, 100, 100, 255, 255); // Blue for right channel
        SDL_RenderDrawLine(r, lastX, lastYRight, x, yRight);

        lastX = x;
        lastYleft = yLeft;
        lastYRight = yRight;
    }
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255); // Reset color to white
    SDL_RenderDrawLine(r, 0, h/2, w, h/2); // Draw horizontal line at middle
}