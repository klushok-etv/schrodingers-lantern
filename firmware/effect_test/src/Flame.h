#ifndef FLAME_H
#define FLAME_H

#include "Effect.h"
#include <stdint.h>
#include <arduino.h>
#include <FastLED.h>
#ifndef NUM_LEDS
#define NUM_LEDS 35
#endif

class Flame : public Effect
{
private:
    const uint8_t _layer_size = 6;
    uint8_t _cooling;
    uint8_t _sparking;
    CRGBPalette16 gPal;
    byte heat[NUM_LEDS];

public:
    Flame(CRGB*, uint8_t, uint8_t, uint8_t);
    ~Flame();
    void run();
};

#endif