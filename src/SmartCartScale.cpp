#include <SmartCartScale.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711_ADC.h>
#include <stdint.h>
#include <Input.h>
#include <TimedTask.h>

SmartCartScale::SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19, uint16_t calibrationRate = 5000, uint16_t weightUpdateRate = 200, bool reCalibrates = true): 
expectedWeight(0),
currentUnfluctuatedWeight(900),
currentWeight(0), 
calibrationFactor(0),
validationMargin(0.15),
fixedValidationMargin(125), //300?
segmentedExpectedWeight(0),
reCalibrates(reCalibrates),
currentState(VALID),
powerState(1),
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
  Serial.println("The scale will start in 5 seconds...");
  delay(5000);
}
  
void SmartCartScale::updateCurrentWeight() { 
  currentWeight = LoadCells.getData();
  updateScaleStatus();
  Serial.println("Weight: " + String(currentWeight) + " | " + (currentState ? "VALID" : "INVALID") + " | " + expectedWeight + (currentState ? " | " + record.getFluctuationResults(): ""));  //show result only valid
}

void SmartCartScale::updateScaleStatus() {
  segmentedExpectedWeight = ((expectedWeight * validationMargin) + fixedValidationMargin);
  if ((currentWeight > (expectedWeight - segmentedExpectedWeight)) && (currentWeight < (expectedWeight + segmentedExpectedWeight))) {
    currentState = VALID;
    currentUnfluctuatedWeight = expectedWeight;
    record.reset(expectedWeight);
  }
  else currentState = AWAITING;
}

void SmartCartScale::calibrate() {
  blockUntil('r');
  LoadCells.tare();
  Serial.println("Value of known mass: ");
  float knownsMass = readFloatBlock(); //2lb/907grams microphone, 200g/0.44lb phone
  LoadCells.refreshDataSet();
  calibrationFactor = LoadCells.getNewCalibration(knownsMass); //returns and sets
  displayCalibrationFactor();
}

void SmartCartScale::reCalibrate() {
  if (currentState == VALID && reCalibrates && currentUnfluctuatedWeight != 0) { 
    calibrationFactor = LoadCells.getNewCalibration(currentUnfluctuatedWeight);
    displayCalibrationFactor();
  }
}

void SmartCartScale::displayCalibrationFactor() {
  Serial.println("New calibrationFactor(for expected weight of: " + String(this->expectedWeight) + ") ->" + String(this->calibrationFactor));
}

void SmartCartScale::update() {
  if (powerState) {
    LoadCells.update();
    record.update(currentWeight);
    timedWeightUpdate.invoke();
    timedCalibration.invoke();
  }
}

void SmartCartScale::blockUntil(char inp) {
  // bool resume = false;
  // char in;
  // while (resume == false) {

  // }
}

void SmartCartScale::turnOn() {
  if (powerState == 0) {
    powerState = 1;
    LoadCells.powerUp();
  }
}

void SmartCartScale::turnOff() {
  if (powerState) {
    powerState = 0;
    LoadCells.powerDown();
  }
}

void SmartCartScale::restart(uint16_t offDelay) {
  turnOff();
  delay(offDelay);
  turnOn();
}

void SmartCartScale::interact() {
  if (Serial.available()) {
    char in = Serial.read();
  }
}