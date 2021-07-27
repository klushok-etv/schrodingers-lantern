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
//    22-Jul-2021     bdenouden   OTA & mDNS added
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
#include "credentials.h"
#include <FastLED.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef CREDENTIALS_H
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY credentials.h.template TO credentials.h"
#pragma error "AND CONFIGURE YOUR CREDENTIALS"
#pragma error "!!!!!!!!"
#endif

// led config
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

#define PAR_FILE "/par.dat"
uint8_t rgb[] = {163, 0, 0}; // default color
uint8_t brightness = 255;

// todo make lantern animation with redirect button to home
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  CRGB bootColor = CRGB::Green; // unless errors occur, boots correctly
  pinMode(BTN_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("Starting Lantern");

  // Initialize leds
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    blinkLantern(4, 1000, CRGB::Red);
    delay(1000);
    ESP.restart();
  }

  // show files saved on SPIFFS
  listDir(SPIFFS, "/", 1);

  // start wifiManager or webUI
  if (!WiFiManagerBegin(AP_SSID, AP_PASS))
  {
    // setup wifimanager config portal
    setupWMWebpages();
    bootColor = CRGB::Blue;
  }
  else
  {
    // Initialize webpages and OTA
    setupWebPages();
    setupOTA();

    // setup mDNS
    if (!MDNS.begin(HOSTNAME))
    {
      Serial.println("Error starting mDNS");
      bootColor = CRGB::Orange;
    }
  }

  server.onNotFound(notFound);
  Serial.println("starting server");
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());

  // get saved parameters from SPIFFS
  char params[5];
  getParam(params);
  rgb[0] = params[0]; rgb[1] = params[1]; rgb[2] = params[2];
  brightness = params[3];

  // indicate succesfull boot
  blinkLantern(2, 500, bootColor);

  // set brightness according to SPIFFS parameter
  FastLED.setBrightness(brightness);
}

void setupOTA()
{
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA
  .onStart([]()
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else
    { // U_SPIFFS
      type = "filesystem";
      SPIFFS.end(); // If updating SPIFFS, unmount SPIFFS using SPIFFS.end()
    }
    Serial.println("Start updating " + type);
  })
  .onEnd([]()
  {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

void blinkLantern(uint8_t n, unsigned int ms, CRGB color)
{
  for (int j = 0; j < n; j++)
  {
    for (int i = NUM_LEDS - 1; i >= 0; --i)
    {
      leds[i] = color;
    }
    FastLED.show();
    FastLED.delay(ms);
    FastLED.clear(true);
    FastLED.delay(ms);
  }
}

void turnOff()
{
  FastLED.clear(true);
  state = false;
  Serial.println("Leds turned off");
}

void turnOn()
{
  for (int i = NUM_LEDS - 1; i >= 0; --i)
  {
    leds[i] = CRGB(rgb[0], rgb[1], rgb[2]);
  }
  FastLED.show();
  state = true;
  Serial.println("Leds turned on");
}

void loop()
{
  ArduinoOTA.handle();

  // check if btn has rising edge
  bool curBtnState = digitalRead(BTN_PIN);
  if (curBtnState && !prev_btn_state)
  {
    // toggle
    prev_btn_state = curBtnState;
    if (state) turnOff();
    else turnOn();
  }
  else
  {
    prev_btn_state = curBtnState;
  }
}
