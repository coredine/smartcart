#pragma once
#include <SmartCartScale.h>
#include <stdint.h>
#include <Arduino.h>
#include <vector>
#include <TimedTask.h>

struct SimulationPoint {
    String name;
    float weight;

    SimulationPoint(String name, float weight);
    String toString(); 
};

/**
 * https://stackoverflow.com/questions/54585/when-should-you-use-a-class-vs-a-struct-in-c
 */
class ShoppingSimulation { 
    public:
        SmartCartScale *scale;
        TimedTask<ShoppingSimulation> timedUpdate;
        std::vector<SimulationPoint> *simulationPoints;
        uint16_t currentIndex;

        ShoppingSimulation(SmartCartScale *scale, uint16_t updateRate, std::vector<SimulationPoint> *simulationPoints);
        void changePoint();
        void update();
};