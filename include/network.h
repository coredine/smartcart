#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>

wl_status_t connectToStoreWifi(String storeSsid, String storePassword);
wl_status_t connectToStoreWifi();

#endif /* STORAGE_H */