#include "Flame.h"

Flame::Flame(CRGB *leds, uint32_t fps, uint8_t brightness ,uint8_t cooling, uint8_t sparking)
    : Effect(leds, 1000 / fps, brightness)
{
    this->_cooling = cooling;
    this->_sparking = sparking;
    this->_gPal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow);
}

// Based on the fastled example Fire2012WithPalette
void Flame::effectStep()
{
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((this->_cooling * 10) / NUM_LEDS) + 2));
    }

    // overwrite heat for first layer
    byte temp = random8(190, 220);
    for (int i = 0; i < _layer_size; i++)
    {
        heat[i] = temp;
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    // for (int k = NUM_LEDS - 1; k >= 2; k--)
    for (int k = NUM_LEDS - 1; k >= 7; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < this->_sparking)
    {
        int y = random8(7, 20);
        heat[y] = qadd8(heat[y], random8(200, 255));
    }


    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        byte colorindex = scale8(heat[j], 240);
        CRGB color = ColorFromPalette(this->_gPal, colorindex);
        int pixelnumber;
        // if (gReverseDirection) -> default reversed
        pixelnumber = (NUM_LEDS - 1) - j;
        this->_leds[pixelnumber] = color;
    }
    FastLED.show();
}

void Flame::setColors(CRGBPalette16 palette)
{
    this->_gPal = palette;
}