#include <Arduino.h>
#include "globals.h"
#include "error.h"
#include "Siren.h"

Siren::Siren(const SirenSettings& settings) :
	logSize(settings.logSize),
	logIndex(0),
	pin(settings.pin),
	freq(settings.freq),
    durationMinimal(settings.durationMin),
	durationMaximal(settings.durationMax),
	durationMaxInDurationRef(settings.durationMaxInDurationRef),
	durationRef(settings.durationRef),
    intervalDuration(settings.intervalDuration),
	minDelayBetweenTwoTriggers(settings.minDelayBetweenTwoTriggers),
    
    intermittentToneStartTime(0),
	intermittentToneEndTime(0)

	{
		// Initialisation des pins
		pinMode(pin, OUTPUT);
    	digitalWrite(pin, LOW);

		// Initialisation des logs
		logs = new SirenLog[logSize];
		for (int i = 0; i < logSize; ++i) {
    	    logs[i].startTime = 0;
        	logs[i].endTime = 0;
	    }
	}

void Siren::recordSirenTrigger(unsigned long startTime, unsigned long endTime) {
    // Ajouter la nouvelle entrée au tableau
    logs[logIndex].startTime = startTime;
    logs[logIndex].endTime = endTime;

    // Incrémenter l'index du tableau
    logIndex = (logIndex + 1) % logSize;
}

// Gestion de la sirène
void Siren::handlePlay() {
	// Première sonnerie
	if (isLastLogEmpty()) {
		// Première sonnerie
		playIntermittentTone();
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
			playIntermittentTone();
		}
	}
}

// Sonnerie intermittente de durée durationMinimal
void Siren::playIntermittentTone() {
    intermittentToneStartTime = millis();

	// La sirène sonne pendant durationMinimal
    while (millis() - intermittentToneStartTime < durationMinimal) {
        // Jouer une période de sirène
		tone(pin, freq);
		delay(intervalDuration / 2); // Temps de tonalité
		noTone(pin);
		delay(intervalDuration / 2); // Temps de silence
    }

	intermittentToneEndTime = millis();
	// Enregistrer cette sonnerie dans les logs
	recordSirenTrigger(intermittentToneStartTime, intermittentToneEndTime);
}

// Un bip de sirène
void Siren::playQuickTone() {
  tone(pin, freq, intervalDuration);
}

Siren::~Siren() {
    delete[] logs;  // Libération de la mémoire du tableau
}

int positiveModulo(int value, int modulus) {
    return (value % modulus + modulus) % modulus;
}

bool Siren::isLastLogEmpty() const {
    return (logs[logIndex].startTime == 0 && logs[logIndex].endTime == 0);
}

// Détermine si la sonnerie à sonner plus longtemps que "duration"
bool Siren::hasSoundedMoreThan(unsigned long duration) const {
    unsigned long totalDuration = 0;
	int i = logIndex; 

	// Tant que le log actuel correspond à la même sonnerie que celle qui va sonner 
	while (millis() - logs[i].endTime < minDelayBetweenTwoTriggers) {
		// Ajouter la durée de la sonnerie du log actuel
		totalDuration += (logs[i].endTime - logs[i].startTime);
		// Passer au log précédent
		i = positiveModulo(i - 1, logSize); 
		}

	return (totalDuration >= duration);
}


// Détermine si la sonnerie à sonner plus longtemps que "duration" sur la période "durationRef"
bool Siren::hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const {
    unsigned long totalDuration = 0;
	int i = logIndex;
	int j = 0;

	// Tant que le log actuel n'as pas sonné plus tard qu'il y a durationRef et que la liste entière n'as pas été entièrement parcourue 
	while ((logs[i].startTime > millis() - durationRef) && j < logSize) {
		// Ajouter la durée de la sonnerie du log actuel
		totalDuration += (logs[i].endTime - logs[i].startTime);
		// Passer au log précédent
		i = positiveModulo(i - 1, logSize); 
		// Incrémenter le nombre d'élément parcouru dans la liste
		j++;
	}

	return (totalDuration >= duration);
}