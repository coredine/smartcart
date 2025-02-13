#pragma once
#include <stdint.h>
#include <vector>
using namespace std;

class TimedTask {
    public:
        uint64_t timeSnapshot;
        uint16_t executionRateMillis;
        vector<void (*)()> tasks;

        TimedTask(uint16_t executionRateMillis);
        void invoke();
        bool canExecute();
        void addTask(void(*taskPointer)());
};

