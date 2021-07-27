#include "Disco.h"

Disco::Disco(CRGB *leds, uint32_t delay, uint8_t randomness)
    : Effect(leds, delay)
{
    this->_randomness = randomness ? randomness : 1;
    this->_colors = this->_defaultColors;
    this->_colorLength = 3;
}

void Disco::effectStep()
{
    this->fillSolid(CRGB::Black);
    for (uint8_t i = random8(6); i < NUM_LEDS; i += random8(1,this->_randomness))
    {
        this->_leds[i] = this->_colors[random8(this->_colorLength)];
    }
    FastLED.show();
}

void Disco::setColors(CRGB *colorPtr, uint8_t length)
{
    this->_colors = colorPtr;
    this->_colorLength = length;
}