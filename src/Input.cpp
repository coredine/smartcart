#include <Input.h>
#include <Arduino.h>

float readFloatBlock() { //need to be fast?
    while (Serial.available() == 0);
    return Serial.parseFloat();
}

void blockUntil(char inp) {
    Serial.println("Input: " + String(inp) + " to keep going...");
    bool resume = false;
    while (resume == false) if (Serial.available()) Serial.read() == inp ? resume = true : true;
    Serial.println("\nUnblocked...");
}