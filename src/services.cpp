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
    String stateStr = CART_STATE_VALUES[state];
    Serial.println("Trying to tell the backend that the cart is "+stateStr+".");
    HTTPClient http;
    http.begin(config["backend"]["ip"], config["backend"]["port"].as<int>(), "/carts/" + getServiceTag() + "/status");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    auto response = http.POST("state=" + stateStr);

    if(response == 404) {
        throw smart_cart_error("The cart does not exists on the system", "E-0008");
    } else if(response == 400) {
        Serial.println("Invalid CartState, no change will be made.");
        return;
    }

    cartState = state;
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