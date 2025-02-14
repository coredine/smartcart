#pragma once
#include <iostream>
#include <Arduino.h>

class DataFluctuationRecord {
  public:
    float originalValue;
    float lowestValue;
    float highestValue;

    DataFluctuationRecord(float originalValue);
    void update(float newValue);
    void reset(float newValue);
    String getFluctuationResults();
};