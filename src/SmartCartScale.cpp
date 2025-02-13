#include <SmartCartScale/SmartCartScale.h>
#include <SmartCartScale/ScaleState.h>
#include <SmartCartScale/DataFluctuationRecord.h>
#include <HX711/HX711_ADC.h>
#include <stdint.h>
#include <Input.h>

SmartCartScale::SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19) : LoadCells(dataPin, serialClockPin), record(0) {
  // *timedWeightUpdate.addTask(updateCurrentWeight);
  // *timedCalibration.addTask(reCalibrate);
};

void SmartCartScale::setUpHX711(bool calibrateOnStartup = true, bool reverseNegative = false, uint16_t startingDelay = 5000, uint8_t gain = 128, signed long calibrationFactor = 0) {
  Serial.println("SmartCart scale system starting...");
  LoadCells.begin(gain);
  if (reverseNegative) LoadCells.setReverseOutput();
  LoadCells.start(startingDelay);
  if (calibrateOnStartup) calibrate();
  else {
    LoadCells.tare();
    LoadCells.setCalFactor(calibrationFactor);
  }
};
  
void SmartCartScale::updateCurrentWeight() {
  currentWeight = LoadCells.getData();
  Serial.println("Weight: " + String(currentWeight));
}
  
void SmartCartScale::updateScaleStatus() {
  segmentedExpectedWeight = ((expectedWeight * validationMargin) + fixedValidationMargin);
  if ( (currentWeight > (expectedWeight - segmentedExpectedWeight)) || (currentWeight < (expectedWeight + segmentedExpectedWeight))) {
    currentState = VALID;
    currentUnfluctuatedWeight = expectedWeight;
  }
  else currentState = AWAITING;
}

void SmartCartScale::calibrate() {
  LoadCells.tare();
  Serial.println("Value of known mass: ");
  float knownsMass = readFloatBlock(); //2lb/907grams microphone, 200g/0.44lb phone
  LoadCells.refreshDataSet();
  calibrationFactor = LoadCells.getNewCalibration(knownsMass); //returns and sets
  Serial.println("New calibration factor: " + String(calibrationFactor));
  Serial.println("The scale will start in 5 seconds...");
  delay(5000);
}

// void reCalibrate() {
  
// }