#include <Arduino.h>
#include <SmartCartScale/SmartCartScale.h>

void setup() {
  Serial.begin(9600);
  delay(1000);
  //setUpHX711();
}

void loop() {
  //if (LoadCells.update()) updateCurrentWeight();
}
