#include <Arduino.h>
#include <SmartCartScale.h>
#include <ShoppingSimulation.h>
#include <ShoppingSimulation.h>
#include <vector>

std::vector<SimulationPoint> simPoints = 
{
  SimulationPoint("Microphone", 907.88),
  SimulationPoint("Phone", 195)
};
SmartCartScale scale(23, 19, 5000, 200);
ShoppingSimulation sim(&scale, 10000, &simPoints);

void setup() {
  Serial.begin(57600);
  delay(5000);
  scale.setUpHX711(true, false, 5000, 128, 50);
}

void loop() {
  scale.update();
  sim.update();
}
