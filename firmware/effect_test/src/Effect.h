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
public:
    Effect(CRGB*, uint32_t); // LEDS object, fps/delay
    void run();
    virtual void effectStep()=0; // run effect frame
    ~Effect(){};
    void fillSolid(CRGB);
    virtual void setColors(CRGB*, uint8_t);
    virtual void setColorPalette(CRGBPalette16);
    void setBrightness(uint8_t);
    void setDelay(uint32_t);
    void setFPS(uint32_t);
};
 
#endif