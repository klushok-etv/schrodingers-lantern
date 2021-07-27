#include <FastLED.h>
#include "src/flame.h"
#include "src/RGB_step.h"
#define NUM_LEDS 35
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

Effect* fx;

void setup() {
  Serial.begin(115200);
  Serial.println("started");
  // put your setup code here, to run once:
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);

  //Flame(*leds, fps, brightness, cooling, sparking)
  fx = new Flame(leds, 10, 100, 20, 7);

  // RGB_step(*leds, stepTime, intensity)
  //  fx = new RGB_step(leds, 1000, 10);


}

void loop() {
  // put your main code here, to run repeatedly:
  fx->run();

}
