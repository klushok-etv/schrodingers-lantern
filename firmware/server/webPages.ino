void setupWebPages() {
  server.reset(); // ensure all previous pages are removed

  // NOTE ensure the files have been uploaded to SPIFFS using the dedicated uploader!
  // show index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //Send index.htm with default content type
    request->send(SPIFFS, "/index.html");
  });

  // make css available
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css");
  });

  // make js script available
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js");
  });

  // make logo available
  server.on("/logo.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo.svg");
  });

  // make icon available
  server.on("/favicon.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.svg");
  });
  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.png");
  });

  // make js script available
  server.on("/klushok.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/klushok.svg");
  });


  // api set endpoint
  server.on("/api/set", HTTP_GET, [](AsyncWebServerRequest * request) {
    String message = "{\"status\": \"ERROR\", \"message\":\"missing parameters\"}";
    uint16_t status = 400;
    bool updated = false;

    // check if brightness was set
    if ( request->hasParam("brightness")) {
      brightness = request->getParam("brightness")->value().toInt();
      FastLED.setBrightness(brightness); // set brightness
      turnOn(); // set color
      message = "{\"status\": \"OK\"}";
      status = 200;
      updated = true;
    }

    // check if colors were set
    if ( request->hasParam("red") &&
         request->hasParam("green") &&
         request->hasParam("blue")) {

      rgb[0] = request->getParam("red")->value().toInt();
      rgb[1] = request->getParam("green")->value().toInt();
      rgb[2] = request->getParam("blue")->value().toInt();
      message = "{\"status\": \"OK\"}";
      status = 200;
      updated = true;
      fxState = false;
      turnOn(); // set color
    }

    request->send(status, "application/json", message);
    if (updated) saveParam(); // save parameters to SPIFFS
  });


  // api effect endpoint
  server.on("/api/effect", HTTP_GET, [](AsyncWebServerRequest * request) {
    String message = "{\"status\": \"ERROR\", \"message\":\"missing/invalid parameters\"}";
    uint16_t status = 400;
    bool updated = false;

    if (request->hasParam("id") && request->hasParam("state")) {
      uint8_t id = request->getParam("id")->value().toInt();
      if (id < fxLength) {
        fxIndex = id;
        fxState = request->getParam("state")->value().toInt();
        if (fxState) state = true;
        else if (state) turnOn();
        else turnOff();
        message = "{\"status\":\"OK\"";
        status = 200;
        updated = true;
      }

    }
    request->send(status, "application/json", message);
    if (updated) saveParam(); // save parameters to SPIFFS
  });



  // api toggle endpoint
  server.on("/api/toggle", HTTP_GET, [](AsyncWebServerRequest * request) {
    String message = "{\"status\": \"ERROR\", \"message\":\"missing parameters\"}";
    uint16_t status = 400;
    if (request->hasParam("state")) {
      if (request->getParam("state")->value().toInt() == 1) turnOn();
      else turnOff();
      message = "{\"status\":\"OK\"";
      status = 200;
    }
    request->send(status, "application/json", message);
  });


  // api get info endpoint
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    StaticJsonDocument<256> doc;
    doc["brightness"] = brightness;
    doc["state"] = state ? "on" : "off";
    doc["status"] = "OK";
    doc["RSSI"] = WiFi.RSSI();
    JsonArray json_rgb = doc.createNestedArray("rgb");
    json_rgb.add(rgb[0]);
    json_rgb.add(rgb[1]);
    json_rgb.add(rgb[2]);
    doc["fxState"] = fxState;
    doc["fxIndex"] = fxIndex;
    serializeJson(doc, *response);
    request->send(response);
  });


  // reset wifi credentials
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    clearWMCredentials();
    request->redirect("/");
  });
}
