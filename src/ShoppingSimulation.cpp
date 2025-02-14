#include <ShoppingSimulation.h>
#include <Arduino.h>
#include <stdint.h>
#include <vector>
#include <TimedTask.h>

SimulationPoint::SimulationPoint(String name, float weight) {
    this->name = name;
    this->weight = weight;
}

String SimulationPoint::toString() {
    return "{" + String(name) + ", " + String(weight) + "}";
}

ShoppingSimulation::ShoppingSimulation(SmartCartScale *scale, uint16_t updateRate, std::vector<SimulationPoint> *simulationPoints) 
: timedUpdate(TimedTask<ShoppingSimulation>(updateRate, this)) {
    this->scale = scale;
    this->currentIndex = 0;
    this->simulationPoints = simulationPoints;
    this->timedUpdate.addTask(&ShoppingSimulation::changePoint);
}

void ShoppingSimulation::changePoint() {
    this->scale->expectedWeight = simulationPoints->at(currentIndex).weight;
    Serial.println("[SIM] New expected weight: {" + simulationPoints->at(currentIndex).name + ", " + String(this->scale->expectedWeight) + "}");
    if ((currentIndex + 1) < simulationPoints->size()) currentIndex++;
    else currentIndex = 0;
}

void ShoppingSimulation::update() {
    timedUpdate.invoke();
}