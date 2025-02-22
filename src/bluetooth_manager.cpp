
#include <bluetooth_manager.h>
#include <storage.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

static BLEServer *server;
static BLEService *cartService;

static BLECharacteristic *chJsonItem;
static BLECharacteristic *chSku;
static BLECharacteristic *chAppState;
static BLECharacteristic *chOrder;
static BLECharacteristic *chPaymentInfos;

class ServerCallbacks : public BLEServerCallbacks {
	virtual void onConnect(BLEServer* pServer) {
        Serial.println("Server connected.");
    }
	virtual void onDisconnect(BLEServer* pServer) {
        Serial.println("Disconnected !");
    }
};

class ChSkuCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
    {
        Serial.print("Big data: ");
        Serial.println(pCharacteristic->getValue().c_str());

        // Need to get the item by the SKU in the backend and send it.

        chJsonItem->setValue("{\"id\" : \"SKU-00001\"}");
        chJsonItem->indicate();
    }
};

void initBluetooth()
{
    BLEDevice::init("ESP32");
    server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());
    cartService = server->createService("1cf9e025-5cee-4558-a754-731e27e028ff");

    chJsonItem = cartService->createCharacteristic("20507320-c712-43ed-a240-05d80fd066fd", BLECharacteristic::PROPERTY_INDICATE);
    chSku = cartService->createCharacteristic("c4fa2ae9-d7f4-42ac-8042-afde6dc23568", BLECharacteristic::PROPERTY_WRITE);
    chSku->setCallbacks(new ChSkuCallbacks());

    chAppState = cartService->createCharacteristic("a4ee0286-6010-46b6-8d21-602f1ee38d71", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    chOrder = cartService->createCharacteristic("d923866a-17d1-4dee-829d-426e6b57e2b3", BLECharacteristic::PROPERTY_READ);
    chPaymentInfos = cartService->createCharacteristic("0d3401a6-2d29-427d-9a0d-87dd46b302a4", BLECharacteristic::PROPERTY_WRITE);

    cartService->start();

    auto advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(cartService->getUUID());
    BLEDevice::startAdvertising();
    
    Serial.println("Welcome to the server!");
    Serial.println("Number of connection = "+String(server->getConnectedCount()));
}

void closeBluetooth()
{
    cartService->stop();
    BLEDevice::deinit(true);
}
