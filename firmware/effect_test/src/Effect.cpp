#include "Effect.h"
#include <stdint.h>
#include <FastLED.h>

Effect::Effect(CRGB *leds, uint32_t delay)
{
    this->_leds = leds;
    this->_delay = delay;
}

void Effect::fillSolid(CRGB color)
{
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        this->_leds[i] = color;
    }
    FastLED.show();
}