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

#define NUM_LEDS 10
#define DATA_PIN 5

// Define the array of leds
CRGB leds[NUM_LEDS];



AsyncWebServer server(80);

const char* ssid = SSID_1;
const char* password = PASSWORD_1;

const char* PARAM_MESSAGE = "message";

uint8_t rgb[] = {180, 31, 33};
uint8_t intensity = 100; // only saved to be able to return this value to the web UI, color intensity calculated client side

// todo make lantern animation with redirect button to home
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {

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

      message = "{status: 'OK'}";
    } else {
      message = "{status: 'ERROR', issue:'missing parameters'}";
    }
    request->send(200, "application/json", message);
  });

  // api toggle endpoint
  server.on("/api/toggle", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonBuffer(1024);
    jsonBuffer["heap"] = ESP.getFreeHeap();
    jsonBuffer["ssid"] = WiFi.SSID();
    serializeJson(jsonBuffer, *response);
    request->send(response);
  });

  // api get info endpoint
  server.on("/api/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonBuffer(1024);
    if (request->hasParam("color")) {
      JsonArray json_rgb = jsonBuffer.createNestedArray("rgb");
      json_rgb.add(rgb[0]);
      json_rgb.add(rgb[1]);
      json_rgb.add(rgb[2]);
      jsonBuffer["intensity"] = intensity;
    }
    jsonBuffer["ssid"] = WiFi.SSID();
    jsonBuffer["status"] = "OK";
    serializeJson(jsonBuffer, *response);
    request->send(response);
  });


  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();

  // init leds
  FastLED.addLeds<WS2813, DATA_PIN, BRG>(leds, NUM_LEDS);

}


void loop() {

  // Turn the LED on, then pause
  leds[0] = CRGB(rgb[0], rgb[1], rgb[2]);
  FastLED.show();
//  FastLED.delay(500);
//
//  // Turn the LED on, then pause
//  leds[0] = CRGB::Green;
//  FastLED.show();
//  FastLED.delay(5000);
//  
//  // Turn the LED on, then pause
//  leds[0] = CRGB::Blue;
//  FastLED.show();
//  FastLED.delay(5000);
//
//  // Now turn the LED off, then pause
//  leds[0] = CRGB::Black;
//  FastLED.show();
//  FastLED.delay(5000);
}
