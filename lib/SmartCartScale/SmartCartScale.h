#pragma once
#include <TimedTask.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711_ADC.h>


//forward declaration (https://stackoverflow.com/questions/2133250/x-does-not-name-a-type-error-in-c) 
//(attribute will need to be a pointer)
// template <class T>
// class TimedTask; 

class SmartCartScale {
    public:
        float expectedWeight; //<===== 
        float currentUnfluctuatedWeight; 
        float currentWeight; 
        float calibrationFactor; 
        float validationMargin; 
        float fixedValidationMargin; 
        float segmentedExpectedWeight;
        SCALE_STATE currentState = VALID; 
        HX711_ADC LoadCells; 
        DataFluctuationRecord record; 
        TimedTask<SmartCartScale> timedCalibration;
        TimedTask<SmartCartScale> timedWeightUpdate;

        SmartCartScale(uint8_t dataPin, uint8_t serialClockPin, uint16_t calibrationRate, uint16_t weightUpdateRate);
        void setUpHX711(bool calibrateOnStartup, bool reverseNegative, uint16_t startingDelay, uint8_t gain, float calibrationFactor);
        void updateCurrentWeight();
        void updateScaleStatus();
        void calibrate();
        void reCalibrate();
        void displayCalibrationFactor();
        void update();
};