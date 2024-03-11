// Inclusions des bibliothèques nécessaires
#include <Siren.hpp>		// Gestion de Siren
#include "global.h"	// Paramètres globaux
#include "error.hpp"		// Gestion d'erreurs
#include "stm32g0xx_hal.h"
#include "alt_main.h"
#include "sigfox.hpp"

// Constructeur de Siren
Siren::Siren(const SirenSettings& params) :
//    pin(params.pin),
//    freq(params.freq),
    durationMinimal(params.durationMin),
    durationMaximal(params.durationMax),
    durationMaxInDurationRef(params.durationMaxInDurationRef),
    durationRef(params.durationRef),
	isInterval(params.isInterval),
	intervalDuration(params.intervalDuration),
    minDelayBetweenTwoTriggers(params.minDelayBetweenTwoTriggers),
    logSize(params.logSize),
    logIndex(0),
    logs(new SirenLog[logSize]),
    intermittentToneStartTime(0),
    intermittentToneEndTime(0)
{
    // Pas d'initialisation ici car sinon a lieu avant le setup() du main 
}

// Initialise Siren
void Siren::init() {
	// Initialisation des logs
	for (int i = 0; i < logSize; ++i) {
		logs[i].startTime = 0;
		logs[i].endTime = 0;
	}
}

// Gère la sirène lors de son appel
void Siren::handlePlay() {
	// Première sonnerie
	if (isLastLogEmpty()) {
		// Première sonnerie
		playIntermittentTone(durationMinimal);
	}
	
	// n-ième sonnerie
	else {
		// Si la sirène a trop sonné en continu
		if (hasSoundedMoreThan(durationMaximal)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}

		// Si la sirène a trop sonné pendant une période donnée
		else if (hasSoundedMoreThanXinX(durationMaxInDurationRef, durationRef)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}
		
		// Sinon la sirène doit encore sonner 
		else {
			playIntermittentTone(durationMinimal);
			sendSigfoxAlert(ERROR_CODE);
			setError(errorSirenHasBeenPlayingForTooLong, false);
		}
	}
}

// Sonne de manière intermittente pendant durationMinimal
void Siren::playIntermittentTone(unsigned long duration) {
    //* intermittentToneStartTime = millis();

	//* Sonne pendant durationMinimal
//    while (millis() - intermittentToneStartTime < duration) {
//        // Jouer une période de sirène
//		handleStartBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de tonalité
//		handleStopBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de silence
//    }

	//intermittentToneEndTime = millis();

	// Enregistre l'entrée correspondante dans le journal 'logs'
	recordSirenTrigger(intermittentToneStartTime, intermittentToneEndTime);
}

// Joue un bip de sirène
void Siren::playQuickTone() {
	handleStartBuzzer();
	HAL_Delay(intervalDuration / 2); // Temps de tonalité
	handleStopBuzzer();
}

// Ajoute une entrée (startTime, endTime) au journal 'logs'
void Siren::recordSirenTrigger(unsigned long startTime, unsigned long endTime) {
    // Ajouter la nouvelle entrée au tableau
    logs[logIndex].startTime = startTime;
    logs[logIndex].endTime = endTime;

    // Incrémenter l'index du tableau
    logIndex = (logIndex + 1) % logSize;
}

// Test si la dernière entrée du journal est vide
bool Siren::isLastLogEmpty() const {
    return (logs[logIndex].startTime == 0 && logs[logIndex].endTime == 0);
}

// Détermine si la sonnerie à sonner plus longtemps que "duration"
bool Siren::hasSoundedMoreThan(unsigned long duration) const {
    unsigned long totalDuration = 0;
	int i = logIndex; 

	//* Tant que le log actuel correspond à la même sonnerie que celle qui va sonner
//	while (millis() - logs[i].endTime < minDelayBetweenTwoTriggers) {
//		// Ajouter la durée de la sonnerie du log actuel
//		totalDuration += (logs[i].endTime - logs[i].startTime);
//		// Passer au log précédent
//		i = positiveModulo(i - 1, logSize);
//		}

	return (totalDuration >= duration);
}

// Détermine si la sonnerie à sonner plus longtemps que "duration" sur la période "durationRef"
bool Siren::hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const {
    unsigned long totalDuration = 0;
	int i = logIndex;
	int j = 0;

	//* Tant que le log actuel n'as pas sonné plus tard qu'il y a durationRef et que la liste entière n'as pas été entièrement parcourue
//	while ((logs[i].startTime > millis() - durationRef) && j < logSize) {
//		// Ajouter la durée de la sonnerie du log actuel
//		totalDuration += (logs[i].endTime - logs[i].startTime);
//		// Passer au log précédent
//		i = positiveModulo(i - 1, logSize);
//		// Incrémenter le nombre d'élément parcouru dans la liste
//		j++;
//	}

	return (totalDuration >= duration);
}

// Destructeur de Siren
Siren::~Siren() {
	// Evite les problèmes potentiels
	if (logs != nullptr) {
 	   delete[] logs;  // Libération de la mémoire du tableau
	}
}

// Renvoie une valeur positive de value modulo modulus
int positiveModulo(int value, int modulus) {
    return (value % modulus + modulus) % modulus;
}
