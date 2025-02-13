#include <SmartCartScale/DataFluctuationRecord.h>
#include <iostream>
using namespace std;

DataFluctuationRecord::DataFluctuationRecord(float originalValue) {
  reset(originalValue);
};

void DataFluctuationRecord::update(float newValue) {
  if (newValue < lowestValue) {
    lowestValue = newValue;
  } else if (newValue > highestValue) {
    highestValue = newValue;
  }
}

void DataFluctuationRecord::reset(float newValue) {
  originalValue = newValue;
  lowestValue = newValue;
  highestValue = newValue;
}

std::string DataFluctuationRecord::getFluctuationResults() {
    return "[ " + to_string(lowestValue) + " | " + to_string(originalValue) + " | " + to_string(highestValue) + " ]";
}