#include <Arduino.h>
#include <SmartCartScale.h>
#include <ShoppingSimulation.h>
#include <ShoppingSimulation.h>
#include <vector>

std::vector<SimulationPoint> simPoints = 
{
  SimulationPoint("Microphone", 907.88),
  SimulationPoint("Phone", 195),
  SimulationPoint("Blue Snowball Microphone", 460),
  SimulationPoint("Nothing", 0) 
};
SmartCartScale scale(23, 19, 15000, 100, true);
ShoppingSimulation sim(&scale, 35000, &simPoints);

void setup() {
  Serial.begin(57600);
  delay(8000);
  scale.setUpHX711(false, false, 5000, 128);
}

void loop() {
  sim.update();
  scale.update();
  scale.interact();
}
