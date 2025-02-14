#include <Arduino.h>
#include <SmartCartScale.h>

SmartCartScale scale(23, 19);

void setup() {
  Serial.begin(57600);
  delay(5000);
  scale.setUpHX711(true, false, 5000, 128, 50);
}

void loop() {
  scale.update();
}
