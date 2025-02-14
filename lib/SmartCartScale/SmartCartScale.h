#pragma once
#include <TimedTask.h>
#include <ScaleState.h>
#include <DataFluctuationRecord.h>
#include <HX711/HX711_ADC.h>

template <class T>
class TimedTask; //forward declaration since SmartCartScale is getting declared before TimedTask? (https://stackoverflow.com/questions/2133250/x-does-not-name-a-type-error-in-c)

class SmartCartScale {
    public:
        float expectedWeight = 0; 
        float currentUnfluctuatedWeight= 0; 
        float currentWeight = 0; 
        signed long calibrationFactor; 
        float validationMargin = 0.25; 
        float fixedValidationMargin = 50; 
        float segmentedExpectedWeight;
        SCALE_STATE currentState = VALID; 
        HX711_ADC LoadCells; 
        DataFluctuationRecord record; 
        TimedTask<SmartCartScale> *timedCalibration; //needs to be a pointer because of forward declaration (compiler will expect a future declaration)
        TimedTask<SmartCartScale> *timedWeightUpdate; //needs to be a pointer because of forward declaration

        SmartCartScale(uint8_t dataPin, uint8_t serialClockPin);
        void setUpHX711(bool calibrateOnStartup, bool reverseNegative, uint16_t startingDelay, uint8_t gain, signed long calibrationFactor);
        void updateCurrentWeight();
        void updateScaleStatus();
        void calibrate();
        void reCalibrate();
        void setTimedTasks();
        void update();
};