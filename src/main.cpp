#include <Arduino.h>
#include <SmartCartScale/SmartCartScale.h>
#include <Input.h>
#include <SmartCartScale/TimedTask.h>
#include <SmartCartScale/DataFluctuationRecord.h>

SmartCartScale scale(23, 19);

void setup() {
  Serial.begin(57600);
  delay(5000);
  scale.setUpHX711(true, false, 5000, 128, 0);
}

void loop() {

}
