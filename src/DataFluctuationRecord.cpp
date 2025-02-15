#include <DataFluctuationRecord.h>
#include <iostream>
#include <Arduino.h>

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

String DataFluctuationRecord::getFluctuationResults() {
    return "[ " + String(lowestValue) + " - " + String(originalValue) + " - " + String(highestValue) + " ]";
}