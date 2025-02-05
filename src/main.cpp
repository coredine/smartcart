#include <Arduino.h>

#define BUILT_IN_LED 2

void setup() {
  pinMode(BUILT_IN_LED, OUTPUT);
}

void loop() {
  delay(3000);
  digitalWrite(BUILT_IN_LED, LOW);
  delay(3000);
  digitalWrite(BUILT_IN_LED, HIGH);
}
