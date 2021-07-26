#ifndef RGB_H
#define RGB_H

#include "Effect.h"
#include <stdint.h>
#include <arduino.h>
#include <FastLED.h>

#ifndef NUM_LEDS
#define NUM_LEDS 35
#endif

class RGB_step : public Effect
{
private:
    uint8_t _intensity;
    uint8_t _colorIndex = 0;
    const CRGB _colors[3] = {CRGB::Red, CRGB::Green, CRGB::Blue};

public:
    RGB_step(CRGB*, uint32_t, uint8_t);
    ~RGB_step();
    void run();
};

#endif