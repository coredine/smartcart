#include "storage.h"
#include "exceptions.h"
#include <LittleFS.h>

static String serviceTag;
static bool storageUp = false;
static const char *configPath = "/config.json";

void initStorage(void)
{
    if (storageUp)
    {
        Serial.println("Storage already initialized.");
        return;
    }

    if (!LittleFS.begin(true))
    {
        Serial.println("Enable to mount LittleFS.");
        throw smart_cart_error("Enable to mount LittleFS.", "E-0001");
    }

    storageUp = true;
    return;
}

String generateServiceTag(void)
{
    String tag;
    char character;

    for (int i = 0; i < 7; i++)
    {

        if (random(0, 2) == 1)
        {
            character = random(65, 90);
        }
        else
        {
            character = random(48, 57);
        }

        tag = tag + character;
    }

    return tag;
}

String getServiceTag(void)
{
    if (!serviceTag.isEmpty())
    {
        Serial.println("Returning already cached serviceTag.");
        return serviceTag;
    }

    File file = LittleFS.open("/serviceTag.txt", FILE_READ);
    serviceTag = file.readString();

    if (serviceTag.isEmpty())
    {
        serviceTag = generateServiceTag();
        File writeFile = LittleFS.open("/serviceTag.txt", FILE_WRITE);
        writeFile.print(serviceTag);
        writeFile.flush();
        writeFile.close();
    }

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

    LittleFS.end();
    storageUp = false;
}

void saveConfig(String storeSsid, String storePassword, String backendIp, String backendPort)
{
    JsonDocument config;
    config["wifi"]["ssid"] = storeSsid;
    config["wifi"]["password"] = storePassword;
    config["backend"]["ip"] = backendIp;
    config["backend"]["port"] = backendPort;

    File file = LittleFS.open(configPath, FILE_WRITE);
    file.print(config.as<String>());
    file.flush();
    file.close();
    config.clear();
}

JsonDocument readConfig(void)
{
    if (!LittleFS.exists(configPath))
    {
        return JsonDocument();
    }

    JsonDocument config;
    File file = LittleFS.open(configPath, FILE_READ);
    deserializeJson(config, file.readString());
    file.flush();
    file.close();

    return config;
}