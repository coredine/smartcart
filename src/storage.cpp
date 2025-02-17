#include "storage.h"
#include "exceptions.h"
#include <SD.h>

static String serviceTag;
static bool storageUp = false;

void initStorage(void)
{
    if (storageUp)
    {
        Serial.println("Storage already initialized.");
        return;
    }

    if (!SD.begin())
    {
        Serial.println("Enable to mount SD card.");
        throw smart_cart_error("Enable to mount SD card.", "E-0001");
    }

    if (SD.cardType() == CARD_NONE)
    {
        Serial.println("No SD card found.");
        throw smart_cart_error("No SD card found.", "E-0002");
    }

    storageUp = true;
    return;
}

String getServiceTag(void)
{
    if (!serviceTag.isEmpty())
    {
        Serial.println("Returning already cached serviceTag.");
        return serviceTag;
    }

    File file = SD.open("/serviceTag.txt", FILE_READ);
    serviceTag = file.readString();
    file.flush();
    file.close();
    return serviceTag;
}

void closeStorage(void)
{
    if (!init)
    {
        Serial.println("Storage already closed.");
        return;
    }

    SD.end();
    storageUp = false;
}

void saveConfig(String storeSsid, String storePassword, String backendIp, String backendPort)
{
    JsonDocument config;
    config["wifi"]["ssid"] = storeSsid;
    config["wifi"]["password"] = storePassword;
    config["backend"]["ip"] = backendIp;
    config["backend"]["port"] = backendPort;

    File file = SD.open("/config.json", FILE_WRITE);
    file.print(config.as<String>());
    file.flush();
    file.close();
    config.clear();
}

JsonDocument readConfig(void)
{
    File file = SD.open("/config.json", FILE_READ);
    JsonDocument config;
    deserializeJson(config, file.readString());
    file.flush();
    file.close();
    return config;
}