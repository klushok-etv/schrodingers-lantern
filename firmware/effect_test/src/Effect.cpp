#include "Effect.h"
#include <stdint.h>
#include <FastLED.h>

Effect::Effect(CRGB* leds,  uint8_t delay)
{
    this->_leds=leds;
    this->_delay = delay;
}