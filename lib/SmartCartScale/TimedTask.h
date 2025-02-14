#pragma once
#include <stdint.h>
#include <vector>
#include <Arduino.h>

/**
 * ==> https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl <==
 * THE DEFINITION AND THE DECLARATION NEEDS TO BE IN HEADER FILE WHEN USING TEMPLATES,
 * BECAUSE THE COMPILER COMPILES THEM SEPERATELY......
 */
template <typename T> //or typename T
class TimedTask {
    public:
        uint64_t timeSnapshot;
        uint16_t executionRateMillis;
        T *instance;
        std::vector<void (T::*)()> tasks; //void (*)() != void (Object::*)()

        TimedTask(uint16_t executionRateMillis, T *instance);
        void invoke();
        bool canExecute();
        void addTask(void(T::*taskPointer)());
};

template <typename T>
TimedTask<T>::TimedTask(uint16_t executionRateMillis, T *instance) {
    this->instance = instance;
    this->timeSnapshot = millis();
    this->executionRateMillis = executionRateMillis;
};

//must use '.*' or '->*' to call pointer-to-member function in 'task (...)', e.g. '(... ->* task) (...)'
template <typename T>
void TimedTask<T>::invoke() {
    if (this->canExecute()) {
        for (void (T::*task)() : tasks) (instance->*task)(); //https://stackoverflow.com/questions/10901959/function-pointers-in-c-error-must-use-or-to-call-pointer-to-memb
        this->timeSnapshot = millis();
    }
};

template <typename T>
bool TimedTask<T>::canExecute() {
    return millis() > timeSnapshot + executionRateMillis; //1501 > 1501 + 500
};

template <typename T>
void TimedTask<T>::addTask(void(T::*taskPointer)()) {
    this->tasks.push_back(taskPointer);
};