#include <network.h>
#include <storage.h>

wl_status_t connectToStoreWifi(String storeSsid, String storePassword)
{
    Serial.println("Trying to connect to " + storeSsid + ".");
    WiFi.begin(storeSsid, storePassword);
    auto status = (wl_status_t) WiFi.waitForConnectResult();
    Serial.println("The WiFi status is " + String(status) + ".");
    return status;
}

wl_status_t connectToStoreWifi()
{
    auto config = readConfig();
    return connectToStoreWifi(config["wifi"]["ssid"], config["wifi"]["password"]);
}