#ifndef EFFECT_H
#define EFFECT_H

#include <stdint.h>
#include <FastLED.h>

class Effect
{
protected:
    uint8_t _delay;
    CRGB* _leds;
    unsigned long _prev_t = 0;
public:
    Effect(CRGB*, uint8_t); // fps
    virtual void run()=0; // run effect frame
    ~Effect(){};
};

#endif