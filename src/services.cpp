#include <services.h>
#include <Arduino.h>
#include "cart_init.h"
#include <storage.h>
#include <exceptions.h>
#include <network.h>
#include <bluetooth_manager.h>
#include <HTTPClient.h>

static JsonDocument config;
static CartState cartState;

String CART_STATE_VALUES[] = {
    "OFF",
    "INIT",
    "STAND_BY",
    "RUNNING",
    "NEED_HELP",
    "SECURITY_ISSUE"
};

CartState getCartState(void) {
    return cartState;
}

void monitorStatus(CartState state)
{
    cartState = state;
    String stateStr = CART_STATE_VALUES[state];
    Serial.println("Trying to tell the backend that the cart is "+stateStr+".");
    HTTPClient http;
    http.begin(config["backend"]["ip"], config["backend"]["port"].as<int>(), "/carts/" + getServiceTag() + "/status");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.POST("state=" + stateStr);
}

void powerOn(void)
{
    config = readConfig();

    if (config.isNull())
    {
        Serial.println("Config empty!");
        monitorStatus(CartState::INIT);
        cartInitSetup();
    }

    monitorStatus(CartState::STAND_BY);
}

void powerOff(void)
{
}