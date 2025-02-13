#pragma once
#include <stdint.h>
#include <vector>
#include <SmartCartScale/SmartCartScale.h>
using namespace std;

template <class T> //or typename T
class TimedTask {
    public:
        uint64_t timeSnapshot;
        uint16_t executionRateMillis;
        vector<void (T::*)()> tasks; //void (*)() != void (Object::*)()

        TimedTask(uint16_t executionRateMillis);
        void invoke();
        bool canExecute();
        void addTask(void(T::*taskPointer)());
};

