#include "Effect.h"

Effect::Effect(CRGB *leds, uint32_t delay, uint8_t brightness)
{
    this->_leds = leds;
    this->_delay = delay;
    this->_brightness = brightness;
}

void Effect::fillSolid(CRGB color)
{
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        this->_leds[i] = color;
    }
    FastLED.show();
}

void Effect::run()
{
    // only execute if specified delay has elapsed
    if (millis() - this->_prev_t < this->_delay)
        return;
    this->_prev_t = millis();

    FastLED.setBrightness(this->_brightness);
    this->effectStep();
}

void Effect::setColors(CRGB *, uint8_t)
{
    Serial.println("WARNING - setColors() must be implemented by derived effect classes!");
}

void Effect::setColorPalette(CRGBPalette16 )
{
    Serial.println("WARNING - setColors() must be implemented by derived effect classes!");
}

void Effect::setBrightness(uint8_t brightness){
    this->_brightness = brightness;
}