#include <Arduino.h>
#include <SmartCartScale.h>
#include <ShoppingSimulation.h>
#include <ShoppingSimulation.h>
#include <vector>
#include <Input.h>

std::vector<SimulationPoint> simPoints;
SmartCartScale scale(23, 19, 8000, 100, 15000, true);
ShoppingSimulation sim(&scale, 35000, &simPoints);

void setup() {
  simPoints.push_back(SimulationPoint("Microphone", 907.88));
  // simPoints.push_back(SimulationPoint("Phone", 195));
  // simPoints.push_back(SimulationPoint("Blue Snowball Microphone", 460));
  //simPoints.push_back(SimulationPoint("Nothing", 0));
  Serial.begin(57600);
  delay(8000);
  scale.setUpHX711(false, 5000, 128, -0.83);
}

void loop() {
  sim.update();
  scale.update();
  //scale.interact();
}
