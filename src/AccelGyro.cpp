// Inclusions des bibliothèques nécessaires
#include <Arduino.h>            // Framework Arduino
#include <Adafruit_MPU6050.h>   // Librairie MPU (Accel + Gyro)
#include "error.h"              // Gestion d'erreurs
#include "AccelGyro.h"          // Gestion des capteurs

// Constructeur de AccelGyro
AccelGyro::AccelGyro(const AccelGyroSettings& settings) :
    toleranceGyro(settings.toleranceGyro),
    durationCalibrationGyro(settings.durationCalibrationGyro),
    blinkerPin(settings.blinkerPin),
    refXminGyro(0.0),
    refXmaxGyro(0.0),
    refYminGyro(0.0),
    refYmaxGyro(0.0),
    refZminGyro(0.0),
    refZmaxGyro(0.0),
    mpu(),
    eventGyro(),
    eventAccel(),
    eventTemp()
{}

// Initialisation de AccelGyro
void AccelGyro::init() {
	mpu.begin();
    setupAccel();
	setupGyro();
    calibrateGyro(durationCalibrationGyro);
}

// Calibre le Gyro pendant durationCalibration
void AccelGyro::calibrateGyro(unsigned long calibrationDuration) {
    // Nettoyage initial des événements
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

    // Initialisation des valeurs de référence
    refXminGyro = eventGyro.gyro.x;
    refXmaxGyro = refXminGyro;
    refYminGyro = eventGyro.gyro.y;
    refYmaxGyro = refYminGyro;
    refZminGyro = eventGyro.gyro.z;
    refZmaxGyro = refZminGyro;

    // Calibration
    unsigned long startTime = millis();
    while (millis() - startTime < calibrationDuration) {
        mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
        delay(1); // Délai entre les lectures pour la stabilité

        // Enregistrement des valeurs extrêmes pour chaque axe
        refXminGyro = min(refXminGyro, eventGyro.gyro.x);
        refXmaxGyro = max(refXmaxGyro, eventGyro.gyro.x);
        refYminGyro = min(refYminGyro, eventGyro.gyro.y);
        refYmaxGyro = max(refYmaxGyro, eventGyro.gyro.y);
        refZminGyro = min(refZminGyro, eventGyro.gyro.z);
        refZmaxGyro = max(refZmaxGyro, eventGyro.gyro.z);
    }
}

// Vérifie la connexion avec le MPU6050 et initialise si nécessaire
void AccelGyro::verify() {
    if (!mpu.begin()) {
        setError(errorAccel, true);
        setError(errorGyro, true);
    }
}

// Setup de l'Accel
void AccelGyro::setupAccel(){ 
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(2);               // a calibrer
  mpu.setMotionDetectionDuration(2);                // a calibrer 
  mpu.setInterruptPinLatch(true);                   // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
}


// Setup du Gyro
void AccelGyro::setupGyro() {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

// Détecte les anomalies AccelGyro
void AccelGyro::detectAbnormal() {
    verify();
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

    // Détection de changement d'angle pour chaque axe
    if (
        (eventGyro.gyro.x > refXmaxGyro + toleranceGyro || eventGyro.gyro.x < refXminGyro - toleranceGyro) ||
        (eventGyro.gyro.y > refYmaxGyro + toleranceGyro || eventGyro.gyro.y < refYminGyro - toleranceGyro) ||
        (eventGyro.gyro.z > refZmaxGyro + toleranceGyro || eventGyro.gyro.z < refZminGyro - toleranceGyro)
    ) {
        setError(errorGyro, true);
    } else {
        setError(errorGyro, false);
    }
    
    // Détection de vibration (utilisation de l'accéléromètre)
    if(mpu.getMotionInterruptStatus()) {    //detects accelerations, not motion!
        setError(errorAccel, true);
    } else {
        setError(errorAccel, false);
    }
}

// Affiche les données du gyroscope et de l'accéléromètre
void AccelGyro::serialOutput() {
    // Récupère les évènements de l'AccelGyro
	mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

	// Gyroscope : X,Y,Z avec seuils de tolérance
	Serial.print("\nGyroscope: \n");
	Serial.print("GX: "); Serial.print(eventGyro.gyro.x);
	Serial.print(", Tol+: "); Serial.print(refXmaxGyro + toleranceGyro);
	Serial.print(", Tol-: "); Serial.print(refXminGyro - toleranceGyro);

	Serial.print("\nGY: "); Serial.print(eventGyro.gyro.y);
	Serial.print(", Tol+: "); Serial.print(refYmaxGyro + toleranceGyro);
	Serial.print(", Tol-: "); Serial.print(refYminGyro - toleranceGyro);

	Serial.print("\nGZ: "); Serial.print(eventGyro.gyro.z);
	Serial.print(", Tol+: "); Serial.print(refZmaxGyro + toleranceGyro);
	Serial.print(", Tol-: "); Serial.println(refZminGyro - toleranceGyro);

	// Accéléromètre : X, Y, Z 
    Serial.print("\nAccéléromètre:\n");
    Serial.print("AX: "); Serial.print(eventAccel.acceleration.x);
    Serial.print("\nAY: "); Serial.print(eventAccel.acceleration.y);
    Serial.print("\nAZ: "); Serial.print(eventAccel.acceleration.z);

    // Vibrations anormales
    if (isError(errorAccel)) {
        Serial.println("\nDétection de vibrations (shock) !\n");
    }
}