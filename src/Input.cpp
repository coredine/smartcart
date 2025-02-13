#include <Input.h>
#include <Arduino.h>

float readFloatBlock() {
    while (Serial.available() == 0);
    return Serial.parseFloat();
}