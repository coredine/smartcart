#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

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
    server.send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Setup SmartCart</title></head><body> <form method=\"post\" action=\"/init\" style=\"width: 400px; display: flex; flex-direction: column; gap: 10px\"> <h1 style=\"margin: 0px;\">Cart Setup</h1> <div style=\"display: flex; flex-direction: column; gap: 8px;\"> <input name=\"backendIp\" type=\"text\" placeholder=\"Backend IP\" /> <input name=\"ssid\" type=\"text\" placeholder=\"WiFi SSID\" /> <input name=\"password\" type=\"password\" placeholder=\"WiFi Password\" /> <input name=\"serviceTag\" type=\"text\" placeholder=\"Service Tag\" /> </div> <button style=\"width: 80px;\">Submit</button> </form></body></html>");
  });

  server.on("/init", HTTP_POST, []() {
    Serial.println("Launching the init process...");
    auto storeSsid = server.arg("ssid");
    auto wiFiStatus = connectToStoreWifi(storeSsid, server.arg("password"));
    
    Serial.println("WiFi status is "+wiFiStatus);

    switch (wiFiStatus)
    {
    case WL_NO_SSID_AVAIL:
      server.send(404, "application/json", "{\"message\" : \"The network "+storeSsid+" does not exists.\"}");
      return;
    case WL_CONNECT_FAILED:
      server.send(400, "application/json", "{\"message\" : \"Enable to connect to "+storeSsid+", it maybe due to an invalid password.\"}");
      return;
    case WL_CONNECTED:
      Serial.println("WiFi connection establish.");
      break;
    default:
      server.send(500, "application/json", "{\"message\" : \"Internal Server Error.\"}");
      return;
    }

    Serial.println("See serial logs...");

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
