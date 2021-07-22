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
    StaticJsonDocument<256> doc;
    doc["intensity"] = intensity;
    doc["state"] = state ? "on" : "off";
    doc["status"] = "OK";
    doc["RSSI"] = WiFi.RSSI();
    JsonArray json_rgb = doc.createNestedArray("rgb");
    json_rgb.add(rgb[0]);
    json_rgb.add(rgb[1]);
    json_rgb.add(rgb[2]);
//    
//    // jsonBuffer["ssid"] = WiFi.SSID();
//    // jsonBuffer["localIP"] = WiFi.localIP();
//    // jsonBuffer["hostName"] = WiFi.getHostname();
    serializeJson(doc, *response);
    request->send(response);
  });

  // reset wifi credentials
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request){
    clearWMCredentials();
    request->redirect("/");
  });
}
