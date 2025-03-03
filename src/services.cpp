#include <services.h>
#include <Arduino.h>
#include "cart_init.h"
#include <storage.h>
#include <exceptions.h>
#include <network.h>
#include <bluetooth_manager.h>
#include <HTTPClient.h>

JsonDocument config;

#define OFF "OFF"
#define INIT "INIT"
#define STAND_BY "STAND_BY"
#define RUNNING "RUNNING"
#define NEED_HELP "NEED_HELP"
#define SECURITY_ISSUE "SECURITY_ISSUE"

void monitorStatus(String state)
{
    Serial.println("Trying to tell the backend that the cart is "+state+".");
    HTTPClient http;
    http.begin(config["backend"]["ip"], config["backend"]["port"].as<int>(), "/carts/" + getServiceTag() + "/status");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.POST("state=" + state);
}

void powerOn(void)
{
    config = readConfig();

    if (config.isNull())
    {
        Serial.println("Config empty!");
        monitorStatus("INIT");
        cartInitSetup();
    }

    monitorStatus("STAND_BY");
}

void powerOff(void)
{
}