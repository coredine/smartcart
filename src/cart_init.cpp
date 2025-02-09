#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const String ssid = "ESP32";
const String password = "********";

WebServer server(80);

void cartInitSetup(void)
{
  Serial.println("Creating \"" + ssid + "\" access point.");

  if (!WiFi.softAP(ssid, password))
  {
    Serial.println("Soft AP creation failed.");
    while (1);
  }

  Serial.println("The ip Address is : ");
  Serial.println(WiFi.softAPIP());

  server.on("/setup", HTTP_GET, []() {
    server.send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Setup SmartCart</title></head><body> <form method=\"post\" action=\"/init\" style=\"width: 400px; display: flex; flex-direction: column; gap: 10px\"> <h1 style=\"margin: 0px;\">Cart Setup</h1> <div style=\"display: flex; flex-direction: column; gap: 8px;\"> <input name=\"backendIp\" type=\"text\" placeholder=\"Backend IP\" /> <input name=\"ssid\" type=\"text\" placeholder=\"WiFi SSID\" /> <input name=\"password\" type=\"password\" placeholder=\"WiFi Password\" /> <input name=\"serviceTag\" type=\"text\" placeholder=\"Service Tag\" /> </div> <button style=\"width: 80px;\">Submit</button> </form></body></html>");
  });

  server.on("/init", HTTP_POST, []() {
    Serial.println("Launching the init process...");
    // Need to connect to the WiFi
    auto storeSsid = server.arg("ssid");
    auto storePassword = server.arg("password"); 

    // not fully functionnal
    WiFi.begin(storeSsid, storePassword);

    auto connectionSuccess = false;
    for(auto i = 0; i++; i < 5) {
      delay(500);
      Serial.println("Attempting to connect to "+storeSsid+"...");
  
      if(WiFi.status() == WL_CONNECTED) {
        connectionSuccess = true;
        break;
      }
    }

    if(!connectionSuccess) {
      server.send(403, "application/json", "{\"message\" : \"Unable to connect to "+storeSsid+" network.\"}");
      return;
    }

    // Then try to request the server
    auto backendIp = server.arg("backendIp");

    // if response success ...
    auto serviceTag = server.arg("serviceTag");

    // else ...

    server.send(200, "text/plain", "See serial logs...");
  });

  server.onNotFound([]() {
    server.send(404, "application/json", "{\"message\": \"Not Found\"}");
  });

  server.begin();
}

void cartInitLoop(void)
{
  server.handleClient();
}
