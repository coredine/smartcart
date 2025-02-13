#include <Arduino.h>
#include <SmartCartScale/SmartCartScale.h>
#include <TimedTask.h>

TimedTask five(5000);
TimedTask ten(10000);

void slt1() {
  Serial.println("oui");
}

void slt2() {
  Serial.println("non");
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  five.addTask(slt1);
  five.addTask(slt2);
  ten.addTask(slt2);
  //setUpHX711();
}

void loop() {
  five.invoke();
  ten.invoke();
  //if (LoadCells.update()) updateCurrentWeight();
}
