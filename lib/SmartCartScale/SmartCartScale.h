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
        bool dataReady;
        bool powerState;
        uint16_t resartDelay;
        HX711_ADC LoadCells; 
        DataFluctuationRecord record; 
        TimedTask<SmartCartScale> timedCalibration;
        TimedTask<SmartCartScale> timedWeightUpdate;
        TimedTask<SmartCartScale> timedRestart;

        SmartCartScale(uint8_t dataPin = 23, uint8_t serialClockPin = 19, uint16_t calibrationRate = 12000, uint16_t weightUpdateRate = 300, uint16_t restartRate = 15000, bool reCalibrates = false);
        void setUpHX711(bool calibrateOnStartup = true, uint16_t startingDelay = 5000, uint8_t gain = 128, float calibrationFactor = -0.83);
        void updateCurrentWeight();
        void updateScaleStatus();
        void tare();
        void calibrate();
        void reCalibrate();
        void displayCalibrationFactor();
        void update();

        void turnOn(); 
        void turnOff(); 
        void restart(); 
        void interact(); 
};