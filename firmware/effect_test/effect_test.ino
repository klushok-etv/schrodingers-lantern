#include <FastLED.h>
#include "src/flame.h"
#include "src/RGB_step.h"
#define NUM_LEDS 35
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

Effect* fx;
CRGB c2[5] = {CRGB::Pink, CRGB::Orange, CRGB::Purple, CRGB::Blue, CRGB::Yellow};

bool toggle = true;
unsigned long t_start;

void setup() {
  Serial.begin(115200);
  Serial.println("started");
  // put your setup code here, to run once:
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);

  //Flame(*leds, fps, cooling, sparking)
  fx = new Flame(leds, 10, 20, 7);
  FastLED.setBrightness(50);

  // RGB_step(*leds, stepTime)
  //    fx = new RGB_step(leds, 1000);

  t_start = millis();

}

void loop() {
  // put your main code here, to run repeatedly:
  fx->run();
  if (millis() - t_start > 15000 && toggle) {
    delete fx;
    fx = new RGB_step(leds, 1000);
    toggle = false;
  }
  else if (millis() - t_start > 10000) {
    FastLED.setBrightness(255);
  }
  else if (millis() - t_start > 5000) {
    //    fx->setColors(c2,5);
    fx->setColorPalette(CRGBPalette16(CRGB::Black, CRGB::Purple, CRGB::Pink));
  }


}
