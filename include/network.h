#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

wl_status_t connectToStoreWifi(String storeSsid, String storePassword);
wl_status_t connectToStoreWifi(JsonDocument config);

#endif /* STORAGE_H */