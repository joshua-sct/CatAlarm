#include <Arduino.h>
#include "globals.h"
#include <Adafruit_MPU6050.h> // Accel + Gyro
#include <Adafruit_Sensor.h> // Accel + Gyro
#include "sensors.h"
#include "error.h"

AccelGyro::AccelGyro(const AccelGyroSettings& settings) :
    toleranceAccel(settings.toleranceAccel),
    toleranceGyro(settings.toleranceGyro),
    durationCalibration(settings.durationCalibration),
    blinkerPin(settings.blinkerPin),
    mpu(),
    refXminGyro(0.0),
    refXmaxGyro(0.0),
    refYminGyro(0.0),
    refYmaxGyro(0.0),
    refZminGyro(0.0),
    refZmaxGyro(0.0),
    refAccel(0.0),
    eventGyro(),
    eventAccel(),
    eventTemp()
{}

void AccelGyro::init() {
	mpu.begin();
    setupShockDetection();
	setupTiltDetection();
    calibrateGyro();
}

// Ajouter ici le reste des fonctions définies plus haut, avec des commentaires expliquant leur fonctionnement
// Calibre l'orientation pendant les premières secondes définies par durationCalibration
void AccelGyro::calibrateGyro() {
    // Nettoyage initial des événements
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
    refAccel = abs(eventAccel.acceleration.x) + abs(eventAccel.acceleration.y) + abs(eventAccel.acceleration.z);
    delay(100); // Délai pour stabiliser le capteur

    // Initialisation des valeurs de référence /#
    refXminGyro = eventGyro.gyro.x;
    refXmaxGyro = refXminGyro;
    refYminGyro = eventGyro.gyro.y;
    refYmaxGyro = refYminGyro;
    refZminGyro = eventGyro.gyro.z;
    refZmaxGyro = refZminGyro;

    unsigned long startTime = millis();
    while (millis() - startTime < durationCalibration) {
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

void AccelGyro::setupShockDetection(){ //TODO VVVV
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(2);// a calibrer
  mpu.setMotionDetectionDuration(2);// a calibrer
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
}


// Configuration de la détection de changement d'angle
void AccelGyro::setupTiltDetection() {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

// Détecte les mouvements à l'aide du gyroscope
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
	mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

	// Gyroscope : X,Y,Z avec seuils de tolérance
	Serial.print("\nGX: "); Serial.print(eventGyro.gyro.x);
	Serial.print(" Tol+: "); Serial.print(refXmaxGyro + toleranceGyro);
	Serial.print(" Tol-: "); Serial.print(refXminGyro - toleranceGyro);

	Serial.print(" GY: "); Serial.print(eventGyro.gyro.y);
	Serial.print(" Tol+: "); Serial.print(refYmaxGyro + toleranceGyro);
	Serial.print(" Tol-: "); Serial.print(refYminGyro - toleranceGyro);

	Serial.print(" GZ: "); Serial.print(eventGyro.gyro.z);
	Serial.print(" Tol+: "); Serial.print(refZmaxGyro + toleranceGyro);
	Serial.print(" Tol-: "); Serial.println(refZminGyro - toleranceGyro);

	// Accéléromètre :  avec seuils de tolérance
	Serial.print("Vibration Ref: "); Serial.print(refAccel);
	Serial.print(" Tol+: "); Serial.print(refAccel + toleranceAccel);
	Serial.print(" Tol-: "); Serial.println(refAccel - toleranceAccel);
}