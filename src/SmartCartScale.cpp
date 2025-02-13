#include <SmartCartScale/SmartCartScale.h>
#include <SmartCartScale/ScaleState.h>
#include <SmartCartScale/DataFluctuationRecord.h>
#include <HX711/HX711_ADC.h>
#include <stdint.h>
#include <Input.h>

const bool TARE_ON_STARTUP = true; 
const bool CALIBRATE_ON_STARTUP = true;
const bool REVERSE_NEGATIVE = false;
const int DATA_PIN = 23; 
const int SERIAL_CLOCK_PIN = 19; 
const int GAIN = 128;  
const uint32_t STARTING_DELAY = 5000; 
const uint32_t UPDATE_RATE_MILLIS = 0;  
const int32_t RECALIBRATION_RATE_MILLIS = 5000; 
uint64_t timeMillis = 0;
float expectedWeight = 0;
float currentUnfluctuatedWeight= 0;
float currentWeight = 0;
float tempCurrentWeight;
float calibrationFactor = 4;
float validationMargin = 0.25;
float fixedValidationMargin = 50; 
float segmentedExpectedWeight;
SCALE_STATE currentState;
HX711_ADC LoadCells(DATA_PIN, SERIAL_CLOCK_PIN);
DataFluctuationRecord record(50);

void setUpHX711() {
  Serial.println("SmartCart scale system starting...");
  LoadCells.begin(GAIN);
  if (REVERSE_NEGATIVE) LoadCells.setReverseOutput();
  LoadCells.start(STARTING_DELAY, TARE_ON_STARTUP);
  if (CALIBRATE_ON_STARTUP) calibrate();
  else LoadCells.setCalFactor(calibrationFactor);
}
  
void updateCurrentWeight() {
  if (millis() > timeMillis + UPDATE_RATE_MILLIS) {
    currentWeight = LoadCells.getData();
    Serial.println("Weight: " + String(currentWeight));
    timeMillis = millis();
  }
}
  
void updateScaleStatus() {
  segmentedExpectedWeight = ((expectedWeight * validationMargin) + fixedValidationMargin);
  if ( (currentWeight > (expectedWeight - segmentedExpectedWeight)) || (currentWeight < (expectedWeight + segmentedExpectedWeight))) 
    currentState = VALID;
  else currentState = AWAITING;
}

void calibrate() {
  LoadCells.tare();
  Serial.println("Value of known mass: ");
  float knownsMass = readFloatBlock(); //2lb/907grams microphone, 200g/0.44lb phone
  LoadCells.refreshDataSet();
  calibrationFactor = LoadCells.getNewCalibration(knownsMass); //returns and sets
  Serial.println("New calibration factor: " + String(calibrationFactor));
  Serial.println("The scale will start in 5 seconds...");
  delay(5000);
}

void reCalibrate() {
  
}