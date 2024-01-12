// Inclusions des bibliothèques nécessaires
#include <Arduino.h>		// Framework Arduino
#include "globals.h"		// Paramètres globaux
#include "error.h"			// Gestion d'erreurs
#include "AccelGyro.h"		// Gestion d'AccelGyro
#include "Siren.h"			// Gestion de Siren

// Déclaration du code d'erreur
uint32_t ERROR_CODE;

// Paramètres d'AccelGyro
AccelGyroSettings myAccelGyroSettings = {
	GYRO_TOLERANCE_ANGLE,
	GYRO_DURATION_CALIB,
	BLINKER_PIN
};

// Paramètres de Siren
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

// Création de l'instance d'AccelGyro
AccelGyro myAccelGyro(myAccelGyroSettings);

// Création de l'instance de la sirène
Siren mySiren(mySirenSettings);

// Fonction de démarrage exécutée une fois au démarrage du microcontrôleur
void setup() {
	ERROR_CODE = 0;

	// Initialisation du port série pour le débogage
	Serial.begin(9600);

	// Initialisation de la sirène
	mySiren.init();

	// 1er setup tone
	mySiren.playQuickTone();

	// Initialisation d'AccelGyro	
	myAccelGyro.init();

	// 2eme setup tone
	mySiren.playQuickTone();
}

// Boucle principale du programme
void loop() {
	// Vérification et détection des mouvements
	myAccelGyro.detectAbnormal();

	// Affichage de données pour le débogage (à commenter en production)
	myAccelGyro.serialOutput();
}
