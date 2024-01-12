#ifndef ACCELGYRO_H
#define ACCELGYRO_H

// Librairie MPU (Accel + Gyro)
#include <Adafruit_MPU6050.h>

// Structure des paramètres d'AccelGyro
struct AccelGyroSettings {
    double toleranceGyro;
    unsigned long durationCalibrationGyro;
    int blinkerPin;
};

// Classe AccelGyro
class AccelGyro {
public:
    // Constructeur
    AccelGyro(const AccelGyroSettings& settings);
    
    // Méthodes publiques
    void init();
    void detectAbnormal();
    void serialOutput();

private:
    // Paramètres de configuration
    double toleranceGyro;
    unsigned long durationCalibrationGyro;
    int blinkerPin;
    
    // Références pour la calibration du gyroscope
    double refXminGyro;
    double refXmaxGyro;
    double refYminGyro;
    double refYmaxGyro;
    double refZminGyro;
    double refZmaxGyro;

    // Gestion capteurs (Accel+Gyro)
    Adafruit_MPU6050 mpu;
    sensors_event_t eventGyro;
    sensors_event_t eventAccel;
    sensors_event_t eventTemp;

    // Méthodes privées
    void calibrateGyro(unsigned long calibrationDuration);
    void verify();
    void setupAccel();
    void setupGyro();
};

#endif //ACCELGYRO_H