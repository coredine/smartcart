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
        bool reCalibrates;
        SCALE_STATE currentState; 
        SCALE_STATE lastState;
        bool powerState;
        HX711_ADC LoadCells; 
        DataFluctuationRecord record; 
        TimedTask<SmartCartScale> timedCalibration;
        TimedTask<SmartCartScale> timedWeightUpdate;

        SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19, uint16_t calibrationRate = 15000, uint16_t weightUpdateRate = 300, bool reCalibrates = false);
        void setUpHX711(bool calibrateOnStartup = true, bool reverseNegative = false, uint16_t startingDelay = 5000, uint8_t gain = 128, float calibrationFactor = -0.83);
        void updateCurrentWeight();
        void updateScaleStatus();
        void calibrate();
        void reCalibrate();
        void displayCalibrationFactor();
        void update();

        void blockUntil(char inp);
        void turnOn(); 
        void turnOff(); 
        void restart(uint16_t offDelay = 5000); 
        void interact(); 
};