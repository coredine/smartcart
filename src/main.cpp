#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "cart_init.h"
#include <storage.h>
#include <exceptions.h>
#include <network.h>

// temporary boolean for activating cart setup
#define IS_CART_SETUP true

const String PROJECT_VERSION = "0.2.1";

void setup(void)
{
  Serial.begin(9600);
  Serial.println("The project version is " + PROJECT_VERSION + ".");

  try {
    initStorage();
  } catch(smart_cart_error &e) {
    Serial.print(String(e.what()));
    while(1);
  }

  auto config = readConfig();

  if(config.isNull()) {
    Serial.println("Config empty!");
    cartInitSetup();
  }

  connectToStoreWifi();
}

void loop(void)
{

}
