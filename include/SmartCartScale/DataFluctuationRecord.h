#pragma once
#include <iostream>

class DataFluctuationRecord {
  public:
    float originalValue;
    float lowestValue;
    float highestValue;

    DataFluctuationRecord(float originalValue);
    void update(float newValue);
    void reset(float newValue);
    std::string getFluctuationResults();
};