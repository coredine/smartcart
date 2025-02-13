#include <TimedTask.h>
#include <Arduino.h>
using namespace std;

template <class T>
TimedTask<T>::TimedTask(uint16_t executionRateMillis) {
    this->timeSnapshot = millis();
    this->executionRateMillis = executionRateMillis;
};

template <class T>
void TimedTask<T>::invoke() {
    if (this->canExecute()) {
        for (void (T::*task)() : tasks) task();
        this->timeSnapshot = millis();
    }
};

template <class T>
bool TimedTask<T>::canExecute() {
    return millis() > timeSnapshot + executionRateMillis; //1501 > 1501 + 500
};

template <class T>
void TimedTask<T>::addTask(void(T::*taskPointer)()) {
    this->tasks.push_back(taskPointer);
};