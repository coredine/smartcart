#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "cart_init.h"
#include <storage.h>
#include <exceptions.h>
#include <network.h>
#include <bluetooth_manager.h>
#include <services.h>

// temporary boolean for activating cart setup
#define IS_CART_SETUP true

const String PROJECT_VERSION = "0.2.1";
static JsonDocument config;

void setup(void)
{
  try
  {

    Serial.begin(9600);
    Serial.println("The project version is " + PROJECT_VERSION + ".");

    initStorage();
    config = readConfig();

    if (!config.isNull() && connectToStoreWifi() != WL_CONNECTED)
    {
      throw smart_cart_error("Not able to connect to WiFi store.", "E-0005");
    }

    powerOn();

    initBluetooth();
  }
  catch (smart_cart_error &e)
  {
    Serial.println("Error : " + String(e.what()));
    Serial.println("Error code : " + String(e.getErrorCode()));
    while (1);
  }
}

void loop(void)
{
}
