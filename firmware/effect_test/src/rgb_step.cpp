#include "RGB_step.h"

RGB_step::RGB_step(CRGB *leds, uint32_t delay)
    : Effect(leds, delay)
{
    this->_colors = this->_defaultColors;
    this->_colorLength = 3;
};

void RGB_step::effectStep()
{
    this->fillSolid(this->_colors[this->_colorIndex % this->_colorLength]);
    this->_colorIndex++;
}

void RGB_step::setColors(CRGB *colorPtr, uint8_t length){
    this->_colors = colorPtr;
    this->_colorLength = length;
}