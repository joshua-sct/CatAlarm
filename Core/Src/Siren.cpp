// Inclusions des bibliothèques nécessaires
#include <Siren.hpp>		// Gestion de Siren
#include "global.h"			// Paramètres globaux
#include "error.hpp"		// Gestion d'erreurs
#include "stm32g0xx_hal.h"
#include "alt_main.h"
#include "sigfox.hpp"
#include "gpio.h"
#include "Log.hpp"

// Settings de Siren
SirenSettings mySirenSettings = {
	SIREN_DURATION_MIN,
	SIREN_DURATION_MAX,
	SIREN_DURATION_MAX_IN_DURATION_REF,
	SIREN_DURATION_REF,
	SIREN_INTERVAL_DURATION,
	SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS,
	SIREN_INTERMITTENT_MODE,
	SIREN_LOG_SIZE
};

// Retourne le singleton
Siren& Siren::getInstance() {
    static Siren instance;
    return instance;
}

// Injection de dépendances
void Siren::init(Log& logInstance) {
    myLog = &logInstance;
	playing = false;
	age = 0;
}

// Routine de démarrage de la sirène
void Siren::handleStart() {
	// Première sonnerie
	if (myLog->isLastEntryEmpty()) {
		// Première sonnerie
		ringStartTimestamp = getTimestamp();
		start();
	}

	// n-ième sonnerie
	else {
		// Si la sirène a trop sonné en continu
		if (myLog->hasRungMoreThan(SIREN_DURATION_MAX)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}

		// Si la sirène a trop sonné pendant une période donnée
		else if (myLog->hasRungMoreThanXinX(SIREN_DURATION_MAX_IN_DURATION_REF, SIREN_DURATION_REF)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}
		
		// Sinon la sirène doit sonner ou encore sonner 
		else {
			// Retenir le Timestamp de démarrage de la sirène si elle commence
			if (!playing) {
				ringStartTimestamp = getTimestamp();
			}
			start();
			setError(errorSirenHasBeenPlayingForTooLong, false);
		}
	}
}

// Routine d'exctinction de la sirène hors interruption
void Siren::handleStop() {
	if (playing) {
		// Duréee minimale buzzer = age*(1/freq) = 100*10ms = 1s
		if (age > 100) {
			// Stop
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			playing = false;
			age = 0;

			// Ajouter le log actuel
			LogEntry entry;
			entry.startTimestamp = ringStartTimestamp;
			entry.stopTimestamp = getTimestamp();
			myLog->addEntry(entry);
		}
		age++;
	}
}

// Routine d'extinction de la sirène lors d'une interruption
void Siren::handleStopInterrupt() {
	if (playing) {
		// Stop
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		playing = false;
		age = 0;

		// Ajouter le log actuel
		LogEntry entry;
		entry.startTimestamp = ringStartTimestamp;
		entry.stopTimestamp = getTimestamp();
		myLog->addEntry(entry);
	}
}

// Démarrage de la sirène
void Siren::start() {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	playing = true;
}

// Joue un bip de sirène
void Siren::playQuickTone() {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	HAL_Delay(SIREN_INTERVAL_DURATION / 2); // Temps de tonalité
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}
