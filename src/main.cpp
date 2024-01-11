// Le reste du code devrait inclure les fonctions pour communiquer avec le module Sigfox
// et pour déclencher les actions appropriées en cas d'erreur ou d'alerte.

// Faire un timer pour handleSiren

// Inclusions des bibliothèques nécessaires
#include <Arduino.h>
#include <SoftwareSerial.h> // Port série logiciel (=> sigfox)
#include <Wire.h> // I2C
#include "globals.h"
#include "error.h"
#include "sigfox.h"
#include "Siren.h"
#include <stdint.h>
#include "sensors.h"

// Déclaration du code d'erreur
uint32_t ERROR_CODE;

SirenSettings mySirenSettings = {
	SIREN_PIN,
	SIREN_FREQ,
	SIREN_DURATION_MIN,
	SIREN_DURATION_MAX,
	SIREN_DURATION_MAX_IN_DURATION_REF,
	SIREN_DURATION_REF,
	SIREN_INTERVAL_DURATION,
	SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS, 
	SIREN_LOG_SIZE
};

AccelGyroSettings myAccelGyroSettings = {
	ACCEL_TOLERANCE_VIBRATION * (PI / 180.0),
	GYRO_TOLERANCE_ANGLE,
	GYRO_DURATION_CALIB,
	BLINKER_PIN
};

// Création de l'instance de la sirène
Siren mySiren(mySirenSettings);

// Création de l'instance d'AccelGyro
AccelGyro myAccelGyro(myAccelGyroSettings);

// Fonction de démarrage exécutée une fois au démarrage du microcontrôleur
void setup() {
	ERROR_CODE = 0;


	// Initialisation du port série pour le débogage
	Serial.begin(9600);

	// Initialisation de la sirène
	mySiren.init();

	// 1er setup tone
	mySiren.playQuickTone();
	
	myAccelGyro.init();

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
	myAccelGyro.detectAbnormal();

	// Affichage de données pour le débogage (à commenter en production)
	myAccelGyro.serialOutput();
}


