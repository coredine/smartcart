
#include "bluetooth_manager.h"
#include "storage.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <HTTPClient.h>
#include <list>
#include "services.h"

#define AS_IDLE "IDLE"
#define AS_SCANNING "SCANNING"
#define AS_CHECKOUT "CHECKOUT"
#define AS_END "END"

static BLEServer *server;
static BLEService *cartService;

static BLECharacteristic *chJsonItem;
static BLECharacteristic *chSku;
static BLECharacteristic *chAppState;
static BLECharacteristic *chOrder;
static BLECharacteristic *chCheckout;
static JsonDocument config;
static JsonDocument order;
static JsonArray productsArray;

enum AppState
{
    IDLE,
    SCANNING,
    CHECKOUT,
    END
};

static AppState appState = AppState::IDLE;
static String backendIp;
static int backendPort;
static double total = 0;

class ServerCallbacks : public BLEServerCallbacks
{
    virtual void onConnect(BLEServer *pServer)
    {
        Serial.println("Server connected.");
    }
    virtual void onDisconnect(BLEServer *pServer)
    {
        Serial.println("Disconnected !");
    }
};

class ChSkuCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
    {
        if (appState != AppState::SCANNING)
        {
            Serial.println("You cannot send a SKU, you are not in the scanning state.");
            return;
        }

        JsonDocument json;
        String rawJson = pCharacteristic->getValue().c_str();
        deserializeJson(json, rawJson);

        String sku = json["sku"];
        String action = json["action"];
        Serial.println("Sku: " + sku);

        if (json["action"] == "DEL")
        {
            for (auto it = productsArray.begin(); it != productsArray.end(); ++it)
            {
                if ((*it)["sku"] == sku)
                {
                    total -= (*it)["price"].as<double>();
                    productsArray.remove(it);
                    break;
                }
            }

            chJsonItem->setValue(rawJson.c_str());
            chJsonItem->indicate();
            Serial.println("Total : " + String(total));
            Serial.println("List size : " + String(productsArray.size()));
            return;
        }

        HTTPClient http;
        http.begin(backendIp, backendPort, "/products/" + sku);
        http.setTimeout(3000);

        auto responseCode = http.GET();
        Serial.println("Backend request status is " + String(responseCode));
        if (responseCode != 200)
        {
            chJsonItem->setValue(String(responseCode).c_str());
        }

        else
        {
            String rawItem = String(http.getString().c_str());
            JsonDocument item;
            deserializeJson(item, rawItem);
            productsArray.add(item);
            total += item["price"].as<double>();
            chJsonItem->setValue(rawItem.c_str());
        }

        chJsonItem->indicate();
        Serial.println("Total : " + String(total));
        Serial.println("List size : " + String(productsArray.size()));
    }
};

class ChAppStateCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
    {
        appState = (AppState)String(pCharacteristic->getValue().c_str()).toInt();

        if (getCartState() != CartState::RUNNING && (appState == SCANNING || appState == CHECKOUT))
        {
            monitorStatus(CartState::RUNNING);
        }

        if (appState == AppState::END)
        {
            total = 0;
            productsArray.clear();
            appState = AppState::IDLE;
            monitorStatus(CartState::STAND_BY);
        }

        Serial.println("The new AppState is " + String(appState));
    }
};

class ChCheckoutCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
    {
        if (appState != AppState::CHECKOUT)
        {
            Serial.println("You cannot send the payment info, you are not in the checkout state.");
            return;
        }

        Serial.println("Welcome to checkout!");

        HTTPClient http;
        http.begin(backendIp, backendPort, "/order/process");
        http.addHeader("Content-Type", "application/json");
        JsonDocument body;
        deserializeJson(body, pCharacteristic->getValue().c_str());
        body["total"] = total;
        body["serviceTag"] = getServiceTag();
        body["products"] = productsArray;

        auto responseStatus = http.POST(body.as<String>());
        Serial.println("Response status " + String(responseStatus));

        chCheckout->setValue(http.getString().c_str());
        chCheckout->indicate();
    }
};

void initBluetooth()
{
    productsArray = order.createNestedArray("products");

    config = readConfig();
    backendIp = config["backend"]["ip"].as<String>();
    backendPort = config["backend"]["port"].as<int>();

    BLEDevice::init(getServiceTag().c_str());
    server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());
    cartService = server->createService("1cf9e025-5cee-4558-a754-731e27e028ff");

    chJsonItem = cartService->createCharacteristic("20507320-c712-43ed-a240-05d80fd066fd", BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_READ);
    chSku = cartService->createCharacteristic("c4fa2ae9-d7f4-42ac-8042-afde6dc23568", BLECharacteristic::PROPERTY_WRITE);
    chSku->setCallbacks(new ChSkuCallbacks());

    chAppState = cartService->createCharacteristic("a4ee0286-6010-46b6-8d21-602f1ee38d71", BLECharacteristic::PROPERTY_WRITE);
    chAppState->setCallbacks(new ChAppStateCallbacks());

    chOrder = cartService->createCharacteristic("d923866a-17d1-4dee-829d-426e6b57e2b3", BLECharacteristic::PROPERTY_READ);
    chCheckout = cartService->createCharacteristic("0d3401a6-2d29-427d-9a0d-87dd46b302a4", BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    chCheckout->setCallbacks(new ChCheckoutCallbacks());

    cartService->start();

    auto advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(cartService->getUUID());
    BLEDevice::startAdvertising();

    Serial.println("\nThe ESP32 Bluetooth Address is: " + String(BLEDevice::getAddress().toString().c_str())+".\n");
}

void closeBluetooth()
{
    cartService->stop();
    BLEDevice::deinit(true);
}
