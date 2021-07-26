#include "Effect.h"
#include "RGB_step.h"
#include <stdint.h>
#include <arduino.h>
#include <FastLED.h>


RGB_step::RGB_step(CRGB *leds, uint32_t delay, uint8_t intensity)
    : Effect(leds, delay)
{
    this->_intensity;
};

void RGB_step::run()
{
    // only execute if specified delay has elapsed
    if (millis() - this->_prev_t < this->_delay)
        return;
    this->_prev_t = millis();
    this->fillSolid(this->_colors[this->_colorIndex % 3]); 
    this->_colorIndex++; 
}
