//---------------------------------------------------------------------------
//
// Klushok ETV - (c) 2021 Bram den Ouden, Tom Salden.  All Rights Reserved.
//
// File:        server.ino
//
// Description:
//
//    A simple server implementation containing
//    - pages served from SPIFFS
//    - missing pages handled as 404
//    -
//    Draws sample effects on a an addressable strip using FastLED
//
// History:
//    15-Jun-2021     bdenouden   First semi complete version
//
//---------------------------------------------------------------------------


#include <Arduino.h>
#include <ESPmDNS.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"
//#include "credentials.h"
#include <FastLED.h>

#ifndef CREDENTIALS_H
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY credentials.h.template TO credentials.h"
#pragma error "AND CONFIGURE YOUR CREDENTIALS"
#pragma error "!!!!!!!!"
#endif

const char* AP_SSID = "Schrodingers Lantern";
const char* AP_PASS = "Schrodinger";
#define HOSTNAME "lantern"


// led configj
#define NUM_LEDS 35
#define DATA_PIN 13

// button config
#define BTN_PIN 15
bool prev_btn_state = false; // default btn value is off
bool state = false;

// Define the array of leds
CRGB leds[NUM_LEDS];

// Define webserver on port 80
AsyncWebServer server(80);

uint8_t rgb[] = {163, 0, 0}; // default color
uint8_t intensity = 100; // only saved to be able to return this value to the web UI, color intensity calculated client side

// todo make lantern animation with redirect button to home
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  bool bootErrors = false;
  pinMode(BTN_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("Starting Lantern");

  // Initialize leds
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
  ////
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    blinkLantern(4, 1000, CRGB::Red);
    bootErrors = true;
  }
  listDir(SPIFFS, "/", 1); // show files saved on SPIFF

  if (!MDNS.begin(HOSTNAME)) {
    Serial.println("Error starting mDNS");
    return;
  }

  //  clearWMCredentials();

  if (!WiFiManagerBegin(AP_SSID, AP_PASS)) {
    // setup wifimanager config portal
    setupWMWebpages();
  } else {
    // Initialize webpages
    setupWebPages();
  }
  server.onNotFound(notFound);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());


  Serial.println("starting server");

  // indicate succesfull boot
  if (!bootErrors) {
    blinkLantern(2, 500, CRGB::Green);
  }
  else {
    Serial.println("Some boot errors occured");
    blinkLantern(2, 500, CRGB::Blue);
  }

}

void blinkLantern(uint8_t n, unsigned int ms, CRGB color) {
  for (int j = 0; j < n; j++) {
    for ( int i = NUM_LEDS - 1; i >= 0; --i) {
      leds[i] = color;
    }
    FastLED.show();
    FastLED.delay(ms);
    FastLED.clear(true);
    FastLED.delay(ms);
  }
}


void turnOff() {
  FastLED.clear(true);
  state = false;
  Serial.println("Leds turned off");
}

void turnOn() {
  for ( int i = NUM_LEDS - 1; i >= 0; --i) {
    leds[i] = CRGB(rgb[0], rgb[1], rgb[2]);
  }
  FastLED.show();
  state = true;
  Serial.println("Leds turned on");
}


void loop() {
  //    ESPAsync_WiFiManager->run();
  // check if btn has rising edge
  bool curBtnState = digitalRead(BTN_PIN);
  if (curBtnState && !prev_btn_state) {
    // toggle
    prev_btn_state = curBtnState;
    if (state) {
      turnOff();
    } else {
      turnOn();
    }
  } else {
    prev_btn_state = curBtnState;
  }
}
