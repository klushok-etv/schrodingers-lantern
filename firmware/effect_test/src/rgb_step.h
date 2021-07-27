#ifndef RGB_H
#define RGB_H

#include "Effect.h"


class RGB_step : public Effect
{
private:
    uint8_t _colorIndex = 0;
    CRGB _defaultColors[3] = {CRGB::Red, CRGB::Green, CRGB::Blue};
    CRGB *_colors;
    uint8_t _colorLength = 3;

public:
    RGB_step(CRGB*, uint32_t);
    ~RGB_step(){};
    void effectStep();

    // ptr to color array, length of array
    void setColors(CRGB*, uint8_t); 
};

#endif