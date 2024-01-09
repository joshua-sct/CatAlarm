#include <Arduino.h>
#include "globals.h"
#include "error.h"
#include "siren.h"

Siren::Siren(int pin, unsigned long freq, unsigned long durationMinimal, unsigned long durationMaximal, unsigned long intervalDuration, unsigned long minDelayBetweenTwoTriggers, unsigned long sirenHasBeenPlayingForTooLongMinimumDelayWithoutError, int logSize) :
	sirenHasBeenPlayingForTooLongLastErrorTime(0),
	sirenHasBeenPlayingForTooLongMinimumDelayWithoutError(sirenHasBeenPlayingForTooLongMinimumDelayWithoutError),
	logSize(logSize),
	logIndex(0),
	pin(pin),
	freq(freq),
    durationMinimal(durationMinimal),
	durationMaximal(durationMaximal),
    intervalDuration(intervalDuration),
	minDelayBetweenTwoTriggers(minDelayBetweenTwoTriggers),
    
	firstRing(true),
    toneState(false),
	currentTime(0),
    intermittentToneStartTime(0),
	eachIntervalStartTime(0),
	intermittentToneEndTime(0)

	{
		// Initialisation des pins
		pinMode(pin, OUTPUT);
    	digitalWrite(pin, LOW);

		// Initialisation des logs
		logs = new SirenLog[logSize];
		for (int i = 0; i < logSize; ++i) {
    	    logs[i].startTime = 0;
        	logs[i].duration = 0;
	    }
	}

void Siren::recordSirenTrigger(unsigned long startTime, unsigned long duration) {
    // Ajouter la nouvelle entrée au tableau
    logs[logIndex].startTime = startTime;
    logs[logIndex].duration = duration;

    // Incrémenter l'index du tableau
    logIndex = (logIndex + 1) % logSize;
}

// Gestion de la sirène
void Siren::handlePlay() {
	currentTime = millis();

	/// if logs[index -1] = 0,0 
	// Si c'est la première sirène
	if (firstRing) {
		intermittentToneStartTime = currentTime;
		playIntermittentTone();
		firstRing = false;
	}

	// S'il n'y avait pas déjà le problème "la sirène a trop sonné"
	else if (!(ERROR_CODE & errorSirenHasBeenPlayingForTooLong)) {
		// Si ça fait trop longtemps que la sirène sonne
		if (intermittentToneStartTime - currentTime > durationMaximal) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
			sirenHasBeenPlayingForTooLongLastErrorTime = currentTime;
		}

		// Si c'est une sirène différente  
		else if (intermittentToneEndTime - currentTime > minDelayBetweenTwoTriggers) {
			// Sonner avec un nouveau départ
			intermittentToneStartTime = currentTime;
			playIntermittentTone();
		}

		// La sirène doit encore sonner car (elle a encore le droit de sonner) ET (c'est la même sirène qu'avant)
		else {
			playIntermittentTone();		
		}
	}
}

// Sonnerie intermittente de durée durationMinimal
void Siren::playIntermittentTone() {
	// Initialisation
    toneState = false;
    currentTime = millis();
    intermittentToneStartTime = currentTime;

	// La sirène sonne jusqu'à (currentTime + durationMinimal)
    while (currentTime - intermittentToneStartTime < durationMinimal) {
		eachIntervalStartTime = currentTime;
        // Si on a dépasser la l'interval de temps d'un tone ou d'un silence (sirène intermittente)
		if (currentTime - eachIntervalStartTime >= intervalDuration) {
			// Basculer la sirène
            if (toneState) {
                tone(pin, freq);
            } else {
                noTone(pin);
            }
            toneState = !toneState;
        }
    }

	// Arrêt de la sirène après durationMinimale
	noTone(pin);
	intermittentToneEndTime = millis();
}

// Un bip de sirène
void Siren::playQuickTone() {
  tone(pin, freq, intervalDuration);
}

Siren::~Siren() {
    delete[] logs;  // Libération de la mémoire du tableau
}

unsigned int positiveModulo(int value, unsigned int modulus) {
    return (value % modulus + modulus) % modulus;
}