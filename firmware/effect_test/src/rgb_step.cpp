#include "RGB_step.h"



RGB_step::RGB_step(CRGB *leds, uint32_t delay, uint8_t brightness)
    : Effect(leds, delay, brightness)
{
    // this->_intensity = intensity;
    // Serial.printf("constructor intensity: %d\n", this->_intensity);
};

void RGB_step::effectStep()
{
    this->fillSolid(this->_colors[this->_colorIndex % 3]);
    this->_colorIndex++;
}
