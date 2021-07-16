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
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "credentials.h"
#include <FastLED.h>

// led config
#define NUM_LEDS 35
#define DATA_PIN 13

// button config
#define BTN_PIN 15
bool prev_btn_state = false; // default btn value is off

bool state = false;

// Define the array of leds
CRGB leds[NUM_LEDS];



AsyncWebServer server(80);

const char* ssid = SSID_1;
const char* password = PASSWORD_1;

const char* PARAM_MESSAGE = "message";

uint8_t rgb[] = {163, 0, 0}; // default color
uint8_t intensity = 100; // only saved to be able to return this value to the web UI, color intensity calculated client side

// todo make lantern animation with redirect button to home
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {

  pinMode(BTN_PIN, INPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  listDir(SPIFFS, "/", 1);
  Serial.println("starting server");

  //      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //          request->send(200, "text/plain", "Hello, world");
  //      });

  // NOTE ensure the files have been uploaded to SPIFFS using the dedicated uploader!
  // show index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/index.html");
  });

  // make css available
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/style.css");
  });

  // make js script available
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/script.js");
  });

  // make logo available
  server.on("/logo.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/logo.svg");
  });


  // make js script available
  server.on("/klushok.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/klushok.svg");
  });

  // api set endpoint
  server.on("/api/set", HTTP_GET, [](AsyncWebServerRequest * request) {
    String message;
    if ( request->hasParam("red") &&
         request->hasParam("green") &&
         request->hasParam("blue") &&
         request->hasParam("intensity")) {

      rgb[0] = request->getParam("red")->value().toInt();
      rgb[1] = request->getParam("green")->value().toInt();
      rgb[2] = request->getParam("blue")->value().toInt();
      intensity = request->getParam("intensity")->value().toInt();

      message = "{\"status\": \"OK\"}";
    } else {
      message = "{\"status\": \"ERROR\", \"message\":\"missing parameters\"}";
    }
    request->send(200, "application/json", message);
    turnOn(); // set color
  });

  // api toggle endpoint
  server.on("/api/toggle", HTTP_GET, [](AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam("state")) {
      Serial.println(request->getParam("state")->value().toInt());
      Serial.println(bool(request->getParam("state")->value().toInt()));

      if (request->getParam("state")->value().toInt() == 1) {
        turnOn();
        message = "{\"status\":\"OK\"";
      }
      else if (request->getParam("state")->value().toInt() == 0) {
        turnOff();
        message = "{\"status\":\"OK\"";
      }
      else {
        message = "{\"status\":\"ERROR\", \"message\":\"Invalid parameter supplied\"";
      }
    } else {
      message = "{\"status\": \"ERROR\", \"message\":\"missing parameters\"}";
    }
    request->send(200, "application/json", message);
  });

  // api get info endpoint
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonBuffer(1024);
    JsonArray json_rgb = jsonBuffer.createNestedArray("rgb");
    json_rgb.add(rgb[0]);
    json_rgb.add(rgb[1]);
    json_rgb.add(rgb[2]);
    jsonBuffer["intensity"] = intensity;
    jsonBuffer["state"] = state ? "on" : "off";
    jsonBuffer["ssid"] = WiFi.SSID();
    jsonBuffer["RSSI"] = WiFi.RSSI();
    jsonBuffer["localIP"] = WiFi.localIP();
    jsonBuffer["hostName"] = WiFi.getHostname();
    
    jsonBuffer["status"] = "OK";
    serializeJson(jsonBuffer, *response);
    request->send(response);
  });

  server.onNotFound(notFound);

  server.begin();

  // init leds
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);

  // indicate succesfull boot
  blinkGreen(2, 500);

}

void blinkGreen(uint8_t n, unsigned int ms) {
  for (int j = 0; j < n; j++) {
    for ( int i = NUM_LEDS - 1; i >= 0; --i) {
      leds[i] = CRGB::Green;
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

  //  // Turn the LED on, then pause
  //  for ( int i = 0; i < NUM_LEDS; i++) {
  //    leds[i] = CRGB(rgb[0], rgb[2], rgb[1]);
  //    FastLED.show();
  //    FastLED.delay(100);
  //  }

}
