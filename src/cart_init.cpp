#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "storage.h"

WiFiClient wifi;
WebServer server(80);

bool setupCompleted = false;
String serviceTag;

void initCart(void);
void notFound(void);

String randomPassword(void) {
  String pass = "";
  
  for(int i = 0; i < 10; i++) {
    char character = random(65, 90);
    pass = pass + character;
  }
  
  return pass;
}

wl_status_t connectToStoreWifi(String storeSsid, String storePassword)
{
  Serial.println("Trying to connect to " + storeSsid + ".");
  WiFi.begin(storeSsid, storePassword);
  return (wl_status_t)WiFi.waitForConnectResult();
}

void cartInitSetup(void)
{
  if(!initStorage()) {
    // Need to show error and block the program.
    while(1);
  }

  Serial.println(readConfig().as<String>());

  String apPassword = randomPassword();
  Serial.println("The Access Point password is "+apPassword);
  serviceTag = getServiceTag();

  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating \"" + serviceTag + "\" access point.");

  if (!WiFi.softAP(serviceTag, apPassword))
  {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }

  Serial.println("The ip Address is : ");
  Serial.println(WiFi.softAPIP());

  server.serveStatic("/setup.html", SD, "/public/setup.html");
  
  server.on("/setup", HTTP_GET, [] {
    server.sendHeader("Location", "/setup.html", true);
    server.send(302, "text/plain", "");
  });

  server.on("/init", HTTP_POST, initCart);
  server.onNotFound(notFound);

  server.begin();
}

void cartInitLoop(void)
{
  server.handleClient();
  delay(2);
}

std::tuple<int, String> requestSystemEntry(String ip, int port, JsonDocument body) {
  HttpClient http(wifi, ip, port);
  http.setTimeout(3000);
  http.post("/carts", "application/json", body.as<String>());
  auto responseStatus = http.responseStatusCode();
  
  Serial.println("Status ="+String(responseStatus));
  String responseBody = "";

  if(responseStatus != -2) {
    responseBody = http.responseBody();
    Serial.println("ResponseBody ="+responseBody);
  }
  
  http.stop();
  http.flush();
  body.clear();
  return {responseStatus, responseBody};
}

void initCart(void)
{
  if(setupCompleted) {
    server.send(403, "application/json", "Cart already setup.");
    return;
  }
  
  Serial.println("Launching the init process...");

  // Connect the ESP32 to the store WiFi.
  auto storeSsid = server.arg("ssid");
  auto storePassword = server.arg("password");
  auto wiFiStatus = connectToStoreWifi(storeSsid, storePassword);

  Serial.println("WiFi status is " + String(wiFiStatus) + ".");

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

  if (responseStatus == -2)
  {
    server.send(408, "application/json", "The SmartCart is enable to join the server. Verify that you have specify the right url or that the server is on the \"" + storeSsid + "\" network.");
    return;
  }

  if (responseStatus == 201)
  {
    saveConfig(storeSsid, storePassword, server.arg("ip"), server.arg("port"));
    setupCompleted = true;
    server.send(201, "application/json", "The cart was successfuly added. The next step is to restart the cart.");
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