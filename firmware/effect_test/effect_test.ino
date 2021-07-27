#include <FastLED.h>
#include "src/Flame.h"
#include "src/RGB_step.h"
#include "src/Disco.h"
#define NUM_LEDS 35
#define DATA_PIN 13
CRGB leds[NUM_LEDS];

// button config
#define BTN_PIN 15
bool prev_btn_state = false; // default btn value is off
bool state = false;

Effect* arr[] = {new Flame(leds, 10, 20, 7), new Disco(leds, 200, 10), new RGB_step(leds, 1000)};
uint8_t fxIndex = 0;
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
  //    fx = new Flame(leds, 10, 20, 7);
  //  FastLED.setBrightness(50);
  // RGB_step(*leds, stepTime)
  //    fx = new RGB_step(leds, 1000);

  //  fx = new Disco(leds, 200, 10);
  t_start = millis();
  fx = arr[fxIndex];

}

void loop() {
  // check if btn has rising edge
  bool curBtnState = digitalRead(BTN_PIN);
  if (curBtnState && !prev_btn_state)
  {
    // toggle
    prev_btn_state = curBtnState;
    fxIndex = (fxIndex + 1) % 3;
    fx = arr[fxIndex];
  }
  else
  {
    prev_btn_state = curBtnState;
  }

  fx->run();

  
  //  if (millis() - t_start > 15000 && toggle) {
  //    delete fx;
  //    fx = new RGB_step(leds, 1000);
  //    toggle = false;
  //  }
  //  else if (millis() - t_start > 10000) {
  //    FastLED.setBrightness(255);
  //  }
  //  else if (millis() - t_start > 5000) {
  //    //    fx->setColors(c2,5);
  //    fx->setColorPalette(CRGBPalette16(CRGB::Black, CRGB::Purple, CRGB::Pink));
  //  }
  //  if (millis() - t_start > 5000) {
  //    fx->setFPS(1);
  //  }




}
