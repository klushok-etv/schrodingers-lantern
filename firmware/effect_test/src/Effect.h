#ifndef EFFECT_H
#define EFFECT_H

#include <stdint.h>
#include <FastLED.h>

#ifndef NUM_LEDS
#define NUM_LEDS 35
#endif

class Effect
{
protected:
    uint32_t _delay;
    CRGB* _leds;
    unsigned long _prev_t = 0;
    uint8_t _brightness;
public:
    Effect(CRGB*, uint32_t, uint8_t); // fps
    void run();
    virtual void effectStep()=0; // run effect frame
    ~Effect(){};
    void fillSolid(CRGB);
};
 
#endif