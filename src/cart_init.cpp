#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SD.h>

const String ssid = "ESP32";
const String password = "********";

WiFiClient wifi;
WebServer server(80);

void initCart(void);
void notFound(void);

wl_status_t connectToStoreWifi(String storeSsid, String storePassword)
{
  Serial.println("Trying to connect to " + storeSsid + ".");
  WiFi.begin(storeSsid, storePassword);
  return (wl_status_t)WiFi.waitForConnectResult();
}

void cartInitSetup(void)
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating \"" + ssid + "\" access point.");

  if (!WiFi.softAP(ssid, password))
  {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }

  Serial.println("The ip Address is : ");
  Serial.println(WiFi.softAPIP());

  if (!SD.begin())
  {
    Serial.println("Enable to mount SD card.");
    return;
  }

  auto cardType = SD.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card found.");
    return;
  }

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

std::tuple<int, String> requestSystemEntry(String backendUrl, JsonDocument body) {
  HttpClient http(wifi, backendUrl, 8080);
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
  Serial.println("Launching the init process...");

  // Connect the ESP32 to the store WiFi.
  auto storeSsid = server.arg("ssid");
  auto wiFiStatus = connectToStoreWifi(storeSsid, server.arg("password"));

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
  body["serviceTag"] = server.arg("serviceTag");
  body["securityCode"] = server.arg("securityCode");

  auto [responseStatus, responseBody] = requestSystemEntry(server.arg("backendUrl"), body);

  if (responseStatus == -2)
  {
    server.send(408, "application/json", "The SmartCart is enable to join the server. Verify that you have specify the right url or that the server is on the \"" + storeSsid + "\" network.");
    return;
  }

  if (responseStatus == 201)
  {
    // step to save the data, etc in the SD card and to block the setup route.
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