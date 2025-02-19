#include <SmartCartScale.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711_ADC.h>
#include <stdint.h>
#include <Input.h>
#include <TimedTask.h>

SmartCartScale::SmartCartScale(uint8_t dataPin, uint8_t serialClockPin, uint16_t calibrationRate, uint16_t weightUpdateRate, uint16_t restartRate, bool reCalibrates): 
expectedWeight(0),
currentUnfluctuatedWeight(900),
currentWeight(0), 
calibrationFactor(0),
validationMargin(0.15),
fixedValidationMargin(100), //300?
segmentedExpectedWeight(0),
reCalibrates(reCalibrates),
currentState(VALID),
lastState(VALID),
dataReady(true),
powerState(1),
resartDelay(restartRate),
LoadCells(dataPin, serialClockPin), 
record(0),
timedCalibration(calibrationRate, this), //auto temp2 = new TimedTask<SmartCartScale>(500);/TimedTask<SmartCartScale> temp1(5000, this);/TimedTask<SmartCartScale> temp2 = TimedTask<SmartCartScale>(500, this);
timedWeightUpdate(weightUpdateRate, this),
timedRestart(15000, this) 
{
  timedCalibration.addTask(&SmartCartScale::reCalibrate); //or (*timedCalibration).addTask();
  timedWeightUpdate.addTask(&SmartCartScale::updateCurrentWeight); //https://stackoverflow.com/questions/67150355/how-do-i-dereference-a-pointer-to-an-object-in-c
  timedRestart.addTask(&SmartCartScale::restart);
}

//LoadCells.setReverseOutput();
void SmartCartScale::setUpHX711(bool calibrateOnStartup, uint16_t startingDelay, uint8_t gain, float calibrationFactor) {
  Serial.println("SmartCart scale system starting...");
  LoadCells.begin(gain);
  LoadCells.start(startingDelay, true);
  if (calibrateOnStartup) calibrate();
  else {
    LoadCells.tare();
    LoadCells.setCalFactor(calibrationFactor);
  };
  Serial.println("System will start in 5 seconds...");
  delay(5000);
}
  
void SmartCartScale::updateCurrentWeight() { 
  if (dataReady) {
    currentWeight = LoadCells.getData();
    dataReady = false;
    updateScaleStatus();
    Serial.println("Weight: " + String(currentWeight) + " | " + (currentState ? "VALID" : "AWAITING") + " | " + expectedWeight);  //show result only valid //(currentState ? " | " + record.getFluctuationResults(): "")
  }
}

void SmartCartScale::updateScaleStatus() {
  segmentedExpectedWeight = ((expectedWeight * validationMargin) + fixedValidationMargin);
  if ((currentWeight > (expectedWeight - segmentedExpectedWeight)) && (currentWeight < (expectedWeight + segmentedExpectedWeight))) {
    currentState = VALID;
    currentUnfluctuatedWeight = expectedWeight;
    if (lastState == AWAITING) record.reset(expectedWeight);
  } else currentState = AWAITING;
  lastState = currentState;
}

/**
 * @note from HX711_ADC EXAMPLE PROGRAM
 */
void SmartCartScale::tare() {
  Serial.println("Press t to start taring process...");
  bool resume = false;
  while (resume == false) {
    LoadCells.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCells.tareNoDelay();
      }
    }
    if (LoadCells.getTareStatus() == true) resume = true;
  }
  Serial.println();
  Serial.println("Tare complete");
}

void SmartCartScale::calibrate() {
  while (!LoadCells.update()); //while update returns 0 (no conversion ready)
  tare();
  Serial.println("Value of known mass: ");
  float knownsMass = readFloatBlock();
  LoadCells.refreshDataSet();
  calibrationFactor = LoadCells.getNewCalibration(knownsMass); 
  displayCalibrationFactor();
}

void SmartCartScale::reCalibrate() {
  if (currentState == VALID && reCalibrates && currentUnfluctuatedWeight > 0) {
    while (!LoadCells.update());
    LoadCells.refreshDataSet();
    calibrationFactor = LoadCells.getNewCalibration(currentUnfluctuatedWeight);
    displayCalibrationFactor();
  }
}

void SmartCartScale::displayCalibrationFactor() {
  Serial.println("\nNew calibrationFactor(for expected weight of: " + String(this->expectedWeight) + "): " + String(this->calibrationFactor));
}

void SmartCartScale::update() {
  if (powerState) {
    if (LoadCells.update()) dataReady = true;
    record.update(currentWeight);
    timedWeightUpdate.invoke();
    timedCalibration.invoke();
    //timedRestart.invoke();
  }
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

void SmartCartScale::restart() {
  turnOff();
  delay(resartDelay);
  turnOn();
}

void SmartCartScale::interact() {
  if (Serial.available()) {
    char in = Serial.read();
    switch (in)
    {
    case 't':
      Serial.println("Tare...");
      break;

    case 'o':
      turnOn();
      Serial.println("HX711 ON...");
      break;

    case 'n':
      turnOff();
      Serial.println("HX711 OFF...");
      break;

    default:
      break;
    }
  }
}