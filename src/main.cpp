// Le reste du code devrait inclure les fonctions pour communiquer avec le module Sigfox
// et pour déclencher les actions appropriées en cas d'erreur ou d'alerte.

// Faire un timer pour handleSiren

// Inclusions des bibliothèques nécessaires
#include <Arduino.h>
#include <SoftwareSerial.h> // Port série logiciel (=> sigfox)
#include <Adafruit_MPU6050.h> // Accel + Gyro
#include <Adafruit_Sensor.h> // Accel + Gyro
#include <Wire.h> // I2C
#include "globals.h"
#include "error.h"
#include "sigfox.h"
#include "Siren.h"
#include <stdint.h>
#include "settings.h"

// Définitions des constantes
#define TOLERANCE_ANGLE_CHANGE 15    // Tolérance de changement d'angle après calibration
#define TOLERANCE_VIBRATION 0.1      // Seuil de détection de vibration (probablement en radians)
#define DURATION_CALIB_GYRO 200     // Durée de calibration du gyroscope en millisecondes
#define BLINKER_PIN 1                // Pin pour la détection de clignotement (recalibration)

// Configuration initiale du MPU6050
Adafruit_MPU6050 mpu;
double refXminGyro, refXmaxGyro, refYminGyro, refYmaxGyro, refZminGyro, refZmaxGyro;
double refAccel;
double toleranceAngle = TOLERANCE_ANGLE_CHANGE * (PI / 180.0);
unsigned long durationCalibration = DURATION_CALIB_GYRO;
sensors_event_t eventGyro, eventAccel, eventTemp;

// Déclarations des fonctions
void calibrateGyro();
void detectGyroMovement();
void gyroGraph();

void setupMPUTiltDetection();
void verifyMPU();
void calibrateAccel();

SirenSettings mySirenSettings = {	SIREN_PIN,
									SIREN_FREQ,
									SIREN_DURATION_MIN,
									SIREN_DURATION_MAX,
									SIREN_DURATION_MAX_IN_DURATION_REF,
									SIREN_DURATION_REF,
									SIREN_INTERVAL_DURATION,
									SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS, 
									SIREN_LOG_SIZE	};

// Création de l'instance de la sirène
Siren mySiren(mySirenSettings);

// Fonction de démarrage exécutée une fois au démarrage du microcontrôleur
void setup() {
	// Initialisation du port série pour le débogage
	Serial.begin(9600);

	// 1er setup tone
	mySiren.playQuickTone();
	
	// Initialisation et calibration des capteurs
	verifyMPU();
	setupMPUTiltDetection();
	calibrateAccel();
	calibrateGyro();

	// 2eme setup tone
	mySiren.playQuickTone();

}

// Boucle principale du programme
void loop() {

	// Processus de recalibration si nécessaire
	/*
	if (digitalRead(BLINKER_PIN) == HIGH) {
		delay(30000); // Attente avant la recalibration
		calibrateGyro();
	}*/

	// Vérification et détection des mouvements
	verifyMPU();
	detectGyroMovement();

	// Affichage de données pour le débogage (à commenter en production)
	// gyroGraph();
}

// Ajouter ici le reste des fonctions définies plus haut, avec des commentaires expliquant leur fonctionnement
// Calibre l'orientation pendant les premières secondes définies par durationCalibration
void calibrateGyro() {
  // Nettoyage initial des événements
  mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
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

// Calibre l'accélération (non utilisée dans le code original)
void calibrateAccel() {
  mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
  refAccel = abs(eventAccel.acceleration.x) + abs(eventAccel.acceleration.y) + abs(eventAccel.acceleration.z);
}

// Vérifie la connexion avec le MPU6050 et initialise si nécessaire
void verifyMPU() {
  if (!mpu.begin()) {
	setError(errorAccel, true);
  }
}

// Configuration de la détection de changement d'angle
void setupMPUTiltDetection() {
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

// Détecte les mouvements à l'aide du gyroscope
void detectGyroMovement() {
  mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

  // Détection de changement d'angle pour chaque axe
  if (
	  (eventGyro.gyro.x > refXmaxGyro + toleranceAngle || eventGyro.gyro.x < refXminGyro - toleranceAngle) ||
	  (eventGyro.gyro.y > refYmaxGyro + toleranceAngle || eventGyro.gyro.y < refYminGyro - toleranceAngle) ||
	  (eventGyro.gyro.z > refZmaxGyro + toleranceAngle || eventGyro.gyro.z < refZminGyro - toleranceAngle)
  ) {
		setError(errorGyro, true);
  } else {
		setError(errorGyro, false);
  }
  
  // Détection de vibration (utilisation de l'accéléromètre)
  double vibration = abs(eventAccel.acceleration.x) + abs(eventAccel.acceleration.y) + abs(eventAccel.acceleration.z);
  if (vibration > refAccel + TOLERANCE_VIBRATION || vibration < refAccel - TOLERANCE_VIBRATION) {
	setError(errorAccel, true);
  }
  refAccel = vibration; // Mise à jour de la valeur de référence pour les vibrations
}

// Affiche les données du gyroscope et de l'accéléromètre
void gyroGraph() {
	mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

	// Gyroscope : X,Y,Z avec seuils de tolérance
	Serial.print("\nGX: "); Serial.print(eventGyro.gyro.x);
	Serial.print(" Tol+: "); Serial.print(refXmaxGyro + toleranceAngle);
	Serial.print(" Tol-: "); Serial.print(refXminGyro - toleranceAngle);

	Serial.print(" GY: "); Serial.print(eventGyro.gyro.y);
	Serial.print(" Tol+: "); Serial.print(refYmaxGyro + toleranceAngle);
	Serial.print(" Tol-: "); Serial.print(refYminGyro - toleranceAngle);

	Serial.print(" GZ: "); Serial.print(eventGyro.gyro.z);
	Serial.print(" Tol+: "); Serial.print(refZmaxGyro + toleranceAngle);
	Serial.print(" Tol-: "); Serial.println(refZminGyro - toleranceAngle);

	// Accéléromètre :  avec seuils de tolérance
	Serial.print("Vibration Ref: "); Serial.print(refAccel);
	Serial.print(" Tol+: "); Serial.print(refAccel + TOLERANCE_VIBRATION);
	Serial.print(" Tol-: "); Serial.println(refAccel - TOLERANCE_VIBRATION);
}