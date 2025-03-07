#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "storage.h"
#include "exceptions.h"
#include "network.h"

WiFiClient wifi;
WebServer server(80);

bool setupCompleted = false;
String serviceTag;

void initCart(void);
void notFound(void);

String randomPassword(void)
{
  String pass = "";

  for (int i = 0; i < 10; i++)
  {
    char character = random(65, 90);
    pass = pass + character;
  }

  return pass;
}

void cartInitSetup(void)
{
  Serial.println("Starting the initialisation process...");
  String apPassword = randomPassword();
  Serial.println("The Access Point password is " + apPassword);
  serviceTag = getServiceTag();

  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating \"" + serviceTag + "\" access point.");

  if (!WiFi.softAP(serviceTag, apPassword))
  {
    throw new smart_cart_error("Soft AP creation failed.", "E-0009");
  }

  Serial.println("The ip Address is : ");
  Serial.println(WiFi.softAPIP());

  server.serveStatic("/setup.html", LittleFS, "/public/setup.html");

  server.on("/setup", HTTP_GET, []
            {
    server.sendHeader("Location", "/setup.html", true);
    server.send(302, "text/plain", ""); });

  server.on("/init", HTTP_POST, initCart);
  server.onNotFound(notFound);

  server.begin();

  while (!setupCompleted)
  {
    server.handleClient();
    delay(2);
  }
}

std::tuple<int, String> requestSystemEntry(String ip, int port, JsonDocument body)
{
  HTTPClient http;
  http.begin(ip, port, "/carts");
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(3000);
  auto responseStatus = http.POST(body.as<String>());

  Serial.println("Status =" + String(responseStatus));
  String responseBody = "";

  if (responseStatus != -1)
  {
    responseBody = http.getString();
    Serial.println("ResponseBody =" + responseBody);
  }

  http.end();
  body.clear();
  return {responseStatus, responseBody};
}

void initCart(void)
{
  if (setupCompleted)
  {
    server.send(403, "application/json", "Cart already setup.");
    return;
  }

  Serial.println("Launching the init process...");

  // Connect the ESP32 to the store WiFi.
  auto storeSsid = server.arg("ssid");
  auto storePassword = server.arg("password");
  auto wiFiStatus = connectToStoreWifi(storeSsid, storePassword);

  switch (wiFiStatus)
  {
  case WL_NO_SSID_AVAIL:
    server.send(404, "application/json", "The network \"" + storeSsid + "\" does not exists.");
    return;
  case WL_CONNECT_FAILED:
    server.send(400, "application/json", "Enable to connect to \"" + storeSsid + "\", it maybe due to an invalid password.");
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
  body["serviceTag"] = serviceTag;
  body["securityCode"] = server.arg("securityCode");

  auto [responseStatus, responseBody] = requestSystemEntry(server.arg("ip"), server.arg("port").toInt(), body);

  if (responseStatus == -1)
  {
    server.send(408, "application/json", "The SmartCart is enable to join the server. Verify that you have specify the right url or that the server is on the \"" + storeSsid + "\" network.");
    return;
  }

  if (responseStatus == 201)
  {
    saveConfig(storeSsid, storePassword, server.arg("ip"), server.arg("port"));
    setupCompleted = true;
    server.send(201, "application/json", "Cart successfuly added, it will restart now...");
    ESP.restart();
    return;
  }
  else
  {
    server.send(responseStatus, "application/json", responseBody);
    return;
  }
}

void notFound(void)
{
  server.send(404, "application/json", "Not Found");
}