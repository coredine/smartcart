#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

#define FORMAT_LITTLEFS_IF_FAILED true

const String ssid = "ESP32";
const String password = "********";

WebServer server(80);

wl_status_t connectToStoreWifi(String storeSsid, String storePassword) {
  Serial.println("Trying to connect to "+storeSsid+".");
  WiFi.begin(storeSsid, storePassword);
  return (wl_status_t) WiFi.waitForConnectResult();
}

void cartInitSetup(void)
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating \"" + ssid + "\" access point.");

  if (!WiFi.softAP(ssid, password))
  {
    Serial.println("Soft AP creation failed.");
    while (1);
  }

  Serial.println("The ip Address is : ");
  Serial.println(WiFi.softAPIP());

  if (!SD.begin()) {
    Serial.println("Enable to mount SD card.");
    return;
  }

  auto cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card found.");
    return;
  }

  server.serveStatic("/setup.html", SD, "/public/setup.html");

  server.on("/setup", HTTP_GET, []() { 
    server.sendHeader("Location", "/setup.html", true);
    server.send(302, "text/plain", "");
  });

  server.on("/init", HTTP_POST, []() {
    Serial.println("Launching the init process...");

    // Connect the ESP32 to the store WiFi.
    auto storeSsid = server.arg("ssid");
    auto wiFiStatus = connectToStoreWifi(storeSsid, server.arg("password"));
    
    Serial.println("WiFi status is " + String(wiFiStatus)+".");

    switch (wiFiStatus)
    {
    case WL_NO_SSID_AVAIL:
      server.send(404, "application/json", "The network \""+storeSsid+"\" does not exists.");
      return;
    case WL_CONNECT_FAILED:
      server.send(400, "application/json", "Enable to connect to \""+storeSsid+"\", it maybe due to an invalid password.");
      return;
    case WL_CONNECTED:
      Serial.println("WiFi connection establish.");
      break;
    default:
      server.send(500, "application/json", "Internal Server Error.");
      return;
    }

    // Call the server to add the cart in the system.
    JsonDocument body;
    body["serviceTag"] = server.arg("serviceTag");
    body["securityCode"] = server.arg("securityCode");

    HTTPClient http;
    http.begin(server.arg("backendUrl")+"/carts");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);
    auto responseStatus = http.POST(body.as<String>());

    if(responseStatus == -1) {
      http.end();
      body.clear();
      server.send(408, "application/json", "The SmartCart is enable to join the server. Verify that you have specify the right url or that the server is on the \""+storeSsid+"\" network.");
      return;
    }

    auto responseBody = http.getString();
    http.end();
    body.clear();

    if(responseStatus == 201) {
      // step to save the data, etc in the SD card and to block the setup route.
      server.send(201, "application/json", "The cart was successfuly added. The next step is to restart the cart.");
      return;
    } else {
      server.send(responseStatus, "application/json", responseBody);
      return;
    }
  });

  server.onNotFound([]() {
    server.send(404, "application/json", "Not Found");
  });

  server.begin();
}

void cartInitLoop(void)
{
  server.handleClient();
}
