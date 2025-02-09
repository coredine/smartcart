#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <cart_init.h>

// temporary boolean for activating cart setup
#define IS_CART_SETUP true

const String PROJECT_VERSION = "0.1.2";

void setup(void)
{
  Serial.begin(9600);
  Serial.println("The project version is " + PROJECT_VERSION + ".");
  if (IS_CART_SETUP)
  {
    cartInitSetup();
  }
}

void loop(void)
{
  if (IS_CART_SETUP)
  {
    cartInitLoop();
  }
}
