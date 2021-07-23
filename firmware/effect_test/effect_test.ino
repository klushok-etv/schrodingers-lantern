#include <FastLED.h>
#include "src/flame.h"
#define NUM_LEDS 35
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

Effect* fx;

void setup() {
  Serial.begin(115200);
  Serial.println("started");
  // put your setup code here, to run once:
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
  
  //Flame(*leds, fps, cooling, sparking)
  fx = new Flame(leds, 12, 15, 2);


}

void loop() {
  // put your main code here, to run repeatedly:
  fx->run();

}
