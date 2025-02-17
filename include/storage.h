#pragma once

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>

String getServiceTag(void);
void initStorage(void);
void closeStorage(void);
void saveConfig(String storeSsid, String storePassword, String backendIp, String backendPort);
JsonDocument readConfig(void);

#endif /* STORAGE_H */