#include <SmartCartScale.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711_ADC.h>
#include <stdint.h>
#include <Input.h>
#include <TimedTask.h>

SmartCartScale::SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19) : LoadCells(dataPin, serialClockPin), record(0) {
  this->setTimedTasks();
}

void SmartCartScale::setUpHX711(bool calibrateOnStartup = true, bool reverseNegative = false, uint16_t startingDelay = 5000, uint8_t gain = 128, signed long calibrationFactor = 0) {
  Serial.println("SmartCart scale system starting...");
  LoadCells.begin(gain);
  if (reverseNegative) LoadCells.setReverseOutput();
  LoadCells.start(startingDelay);
  if (calibrateOnStartup) calibrate();
  else {
    LoadCells.tare();
    LoadCells.setCalFactor(calibrationFactor);
  };
}
  
void SmartCartScale::updateCurrentWeight() { //no error
  currentWeight = LoadCells.getData();
  Serial.println("Weight: " + String(currentWeight) + " || " + record.getFluctuationResults() + " || " + currentState);
}

void SmartCartScale::updateScaleStatus() {
  segmentedExpectedWeight = ((expectedWeight * validationMargin) + fixedValidationMargin);
  if ( (currentWeight > (expectedWeight - segmentedExpectedWeight)) || (currentWeight < (expectedWeight + segmentedExpectedWeight))) {
    currentState = VALID;
    currentUnfluctuatedWeight = expectedWeight;
    record.reset(expectedWeight);
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

void SmartCartScale::reCalibrate() {
  if (currentState == VALID) {
    calibrationFactor = LoadCells.getNewCalibration(currentUnfluctuatedWeight);
    Serial.println("New calibrationFactor: " + calibrationFactor);
  }
}

void SmartCartScale::setTimedTasks() {
  TimedTask<SmartCartScale> temp1(5000, this); //or auto temp2 = new TimedTask<SmartCartScale>(500);
  TimedTask<SmartCartScale> temp2 = TimedTask<SmartCartScale>(500, this);
  timedCalibration = &temp1;
  timedWeightUpdate = &temp2;
  (*timedCalibration).addTask(&SmartCartScale::reCalibrate); //or (*timedCalibration).addTask();
  (*timedWeightUpdate).addTask(&SmartCartScale::updateCurrentWeight); //https://stackoverflow.com/questions/67150355/how-do-i-dereference-a-pointer-to-an-object-in-c
}

void SmartCartScale::update() {
  timedCalibration->invoke();
  timedWeightUpdate->invoke();
}