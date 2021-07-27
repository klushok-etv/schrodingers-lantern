#ifndef DISCO_H
#define DISCO_H

#include "Effect.h"

class Disco : public Effect
{
private:
    CRGB _defaultColors[3] = {CRGB::Blue, CRGB::Green, CRGB::Red};
    CRGB *_colors;
    uint8_t _colorLength = 3;
    uint8_t _randomness;

public:
    Disco(CRGB *, uint32_t, uint8_t);
    ~Disco(){};
    void effectStep();

    // ptr to color array, length of array
    void setColors(CRGB *, uint8_t);
};

#endif
