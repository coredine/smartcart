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
    Serial.println("Backend responded with "+ String(response));
    
    if(response == 404) {
        Serial.println("The cart is not registered in the system.");
        Serial.println("Starting the reset process...");
        deleteConfig();
        Serial.println("Restart the SmartCart...");
        ESP.restart();
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
        cartInitSetup();
    }

    if (connectToStoreWifi(config) != WL_CONNECTED)
    {
      throw smart_cart_error("Not able to connect to WiFi store.", "E-0005");
    }

    monitorStatus(CartState::STAND_BY);
}

void powerOff(void)
{
}