#include "storage.h"
#include <SD.h>

static String serviceTag;
static bool storageUp = false;

bool initStorage(void)
{
    if (storageUp)
    {
        Serial.println("Storage already initialized.");
        return true;
    }

    if (!SD.begin())
    {
        Serial.println("Enable to mount SD card.");
        return false;

    }

    if (SD.cardType() == CARD_NONE)
    {
        Serial.println("No SD card found.");
        return false;
    }

    storageUp = true;
    return true;
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