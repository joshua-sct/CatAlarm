#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

struct AccelGyroSettings {
    double toleranceAccel;
    double toleranceGyro;
    unsigned long durationCalibration;
    int blinkerPin;
};

class AccelGyro {
public:
    AccelGyro(const AccelGyroSettings& settings);

    void init();
    void detectAbnormal();
    void serialOutput();

private:
    double toleranceAccel;
    double toleranceGyro;
    unsigned long durationCalibration;
    int blinkerPin;
    Adafruit_MPU6050 mpu;
    double refXminGyro;
    double refXmaxGyro;
    double refYminGyro;
    double refYmaxGyro;
    double refZminGyro;
    double refZmaxGyro;
    double refAccel;
    sensors_event_t eventGyro;
    sensors_event_t eventAccel;
    sensors_event_t eventTemp;

    void calibrateGyro();
    void verify();
    void setupShockDetection();
    void setupTiltDetection();
};

#endif //SENSOR_H