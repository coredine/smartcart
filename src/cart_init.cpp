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
    server.send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Setup SmartCart</title></head><body> <form style=\"width: 400px; display: flex; flex-direction: column; gap: 10px\"> <h1 style=\"margin: 0px;\">Cart Setup</h1> <div style=\"display: flex; flex-direction: column; gap: 8px;\"> <input name=\"backendIp\" type=\"text\" placeholder=\"Backend IP\" /> <input name=\"ssid\" type=\"text\" placeholder=\"WiFi SSID\" /> <input name=\"password\" type=\"password\" placeholder=\"WiFi Password\" /> <input name=\"serviceTag\" type=\"text\" placeholder=\"Service Tag\" /> </div><button style=\"width: 80px;\">Submit</button> </form></body></html>");
  });

  server.on("/init", HTTP_POST, []() {
    Serial.println(server.arg("backendIp"));
    Serial.println(server.arg("ssid"));
    Serial.println(server.arg("password"));
    Serial.println(server.arg("serviceTag"));
    server.send(200, "text/plain", "See serial log...");
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
