#include <SmartCartScale.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711_ADC.h>
#include <stdint.h>
#include <Input.h>
#include <TimedTask.h>

SmartCartScale::SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19, uint16_t calibrationRate = 5000, uint16_t weightUpdateRate = 200): 
expectedWeight(0),
currentUnfluctuatedWeight(900),
currentWeight(0), 
calibrationFactor(0),
validationMargin(0.25),
fixedValidationMargin(50),
segmentedExpectedWeight(0),
currentState(VALID),
LoadCells(dataPin, serialClockPin), 
record(0),
timedCalibration(calibrationRate, this), //auto temp2 = new TimedTask<SmartCartScale>(500);/TimedTask<SmartCartScale> temp1(5000, this);/TimedTask<SmartCartScale> temp2 = TimedTask<SmartCartScale>(500, this);
timedWeightUpdate(weightUpdateRate, this) 
{
  timedCalibration.addTask(&SmartCartScale::reCalibrate); //or (*timedCalibration).addTask();
  timedWeightUpdate.addTask(&SmartCartScale::updateCurrentWeight); //https://stackoverflow.com/questions/67150355/how-do-i-dereference-a-pointer-to-an-object-in-c
}

void SmartCartScale::setUpHX711(bool calibrateOnStartup = true, bool reverseNegative = false, uint16_t startingDelay = 5000, uint8_t gain = 128, float calibrationFactor = 50) {
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
  
void SmartCartScale::updateCurrentWeight() { 
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
  displayCalibrationFactor();
  Serial.println("The scale will start in 5 seconds...");
  delay(5000);
}

void SmartCartScale::reCalibrate() {
  if (currentState == VALID) {
    calibrationFactor = LoadCells.getNewCalibration(currentUnfluctuatedWeight);
    displayCalibrationFactor();
  }
}

void SmartCartScale::displayCalibrationFactor() {
  Serial.println("New calibrationFactor: " + String(this->calibrationFactor));
}

void SmartCartScale::update() {
  LoadCells.update();
  timedWeightUpdate.invoke();
  timedCalibration.invoke();
}