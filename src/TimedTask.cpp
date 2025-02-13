#include <TimedTask.h>
#include <Arduino.h>
using namespace std;

TimedTask::TimedTask(uint16_t executionRateMillis) {
    this->timeSnapshot = millis();
    this->executionRateMillis = executionRateMillis;
};

void TimedTask::invoke() {
    if (this->canExecute()) {
        for (void (*task)() : tasks) task();
        this->timeSnapshot = millis();
    }
};

bool TimedTask::canExecute() {
    return millis() > timeSnapshot + executionRateMillis; //1501 > 1501 + 500
};

void TimedTask::addTask(void(*taskPointer)()) {
    this->tasks.push_back(taskPointer);
};