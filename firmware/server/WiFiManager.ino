#define WM_CRED_FILE "/wm_cred.dat"

void readCredentials(char *ssid, char *pw)
{
    Serial.printf("Reading file: %s\r\n", WM_CRED_FILE);

    File file = SPIFFS.open(WM_CRED_FILE, "r");
    // Serial.println(SPIFFS.exists(WM_CRED_FILE));
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        // File file = fs.open(WM_CRED_FILE, FILE_WRITE);
        return;
    }

    Serial.println("- read from file:");
    uint8_t row = 0;
    uint8_t col = 0;
    while (file.available())
    {
        char c = file.read();
        Serial.write(c);
        if (c != '\n')
        {
            if (row == 0) ssid[col] = c;
            else pw[col] = c;
            col++;
        }
        else
        {
            col = 0;
            row++;
        }

        // Serial.write(file.read());
        // Serial.println(file.readString());
    }
    file.close();
    // Serial.println(message[1]);
    // Serial.println(message[2]);
    // return (const char*)message;
    // return message;
}

void writeCredentials(const char *ssid, const char *pw)
{
    File file = SPIFFS.open(WM_CRED_FILE, "w");
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }

    if (file.print(ssid) && file.print("\n") && file.print(pw))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
    file.close();
}

bool WiFiManagerBegin()
{
    char ssid[30] = "";
    char pw[30] = "";
    readCredentials(ssid, pw);

    Serial.println(ssid);
    Serial.println(pw);

    if (WiFiConnect(ssid, pw))
    {
        return true;
    }
    else
    {
        int n = scanWifiNetworks(); // todo sort networks by RSSI
        if (n > 0)
            listNetworks(n);
        WiFi.mode(WIFI_AP); // switch to AP mode
        delay(100);

        WiFi.softAP("portal_ssid", "portal_pass");
        return false;
    }
}

bool WiFiConnect(const char *ssid, const char *pw)
{
    WiFi.mode(WIFI_STA);
    delay(200);
    Serial.println("attempting connection");
    Serial.print("ssid: ");
    Serial.println(ssid);
    Serial.print("password: ");
    Serial.println(pw);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pw);
    Serial.print("Connecting");
    uint8_t timeout = 50;
    while (WiFi.status() != WL_CONNECTED)
    {
        yield();
        delay(100);
        Serial.print(".");
        timeout--;
        if (timeout == 0)
            break;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        // already connected
        Serial.println("WiFi Connected!");
        return true;
    }
    return false;
}

int scanWifiNetworks()
{
    WiFi.disconnect();
    delay(200);
    int n = WiFi.scanNetworks(); // number of found networks
    if (n == WIFI_SCAN_FAILED)
    {
        Serial.println("scan failed");
        return 0;
    }
    else if (n == WIFI_SCAN_RUNNING)
    {
        Serial.print("Scanning for networks");
        while (WiFi.scanComplete() == WIFI_SCAN_RUNNING)
        {
            Serial.print(".");
            delay(100);
        }
        Serial.println();
    }
    Serial.print("Number of networks: ");
    Serial.println(n);
    return n;
}

void listNetworks(int n)
{
    Serial.println("-----------------------------");
    for (int i = 0; i < n; i++)
    {
        Serial.print((String)WiFi.SSID(i));
        Serial.print(", RSSI: ");
        Serial.println((String)WiFi.RSSI(i));
    }
    Serial.println("-----------------------------\n");
}

void setupWMWebpages()
{
    // server.reset();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  //Send index.htm with default content type
                  request->send(SPIFFS, "/wm/index.html");
              });

    // make css available
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wm/style.css"); });

    // make js script available
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wm/script.js"); });

    // make js script available
    server.on("/klushok.svg", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/klushok.svg"); });

    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  String json = "[";
                  int n = WiFi.scanComplete();
                  //   if (n == WIFI_SCAN_FAILED) scanWifiNetworks();
                  if (n > 0)
                  {
                      for (int i = 0; i < n; i++)
                      {
                          json += "{";
                          json += "\"rssi\":" + String(WiFi.RSSI(i));
                          json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
                          json += ",\"secure\":" + String(WiFi.encryptionType(i));
                          json += "}";
                          if (i < n - 1)
                              json += ",";
                      }
                  }
                  json += "]";
                  request->send(200, "application/json", json);
              });

    server.on("/set", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  if (request->hasParam("ssid", true) &&
                      request->hasParam("pw", true))
                  {
                      request->send(200, "application/json", "Attempting to connect...");
                      Serial.println("Credentials received");
                      AsyncWebParameter *ssid = request->getParam("ssid", true);
                      AsyncWebParameter *pw = request->getParam("pw", true);

                      //   Serial.println(WiFiConnect(
                      //       ssid->value().c_str(),
                      //       pw->value().c_str()));
                      writeCredentials(
                          ssid->value().c_str(),
                          pw->value().c_str());
                    //   readCredentials();
                  }
                  else
                  {
                      request->send(404, "application/json", "Something went wrong");
                  }
              });
}
