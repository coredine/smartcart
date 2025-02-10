#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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

  server.on("/setup", HTTP_GET, []() {
    server.send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Setup SmartCart</title></head><body> <form method=\"post\" action=\"/init\" style=\"width: 400px; display: flex; flex-direction: column; gap: 10px\"> <h1 style=\"margin: 0px;\">Cart Initialization</h1> <div style=\"display: flex; flex-direction: column; gap: 20px;\"> <div style=\"display: flex; flex-direction: column; gap: 8px;\"> <h2 style=\"margin: 0px;\">Wifi Info</h2> <input required name=\"ssid\" type=\"text\" placeholder=\"SSID*\" /> <input required name=\"password\" type=\"password\" placeholder=\"Password*\" /> </div> <div style=\"display: flex; flex-direction: column; gap: 8px;\"> <h2 style=\"margin: 0px;\">Backend info</h2> <input required name=\"backendUrl\" type=\"text\" placeholder=\"Backend URL*\" /> <!-- For now it is an input, normal it is stored in the cart SD. --> <input required name=\"serviceTag\" type=\"text\" placeholder=\"Service Tag*\" /> <input required name=\"securityCode\" type=\"text\" placeholder=\"Security Code*\" /> </div> </div> <button style=\"width: 70px;\">Submit</button> </form></body></html>");
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

    if(responseStatus == 200) {
      // step to save the data, etc in the SD card and to block the setup route.
      server.send(200, "application/json", "The cart was successfuly added. The next step is to restart the cart.");
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
