#ifndef SIREN_H
#define SIREN_H

#include <stdint.h>
#include "rtc.h"
#include "Log.hpp"

// Structure des paramètres de Siren
struct SirenSettings {
    uint32_t intervalDuration;
    uint32_t minDelayBetweenTwoTriggers;
    uint32_t durationMin;
    uint32_t durationMax;
    uint32_t durationMaxInDurationRef;
    uint32_t durationRef;
    bool intermittentMode;
    int logSize;
};

// Classe Siren
class Siren {
public:
    // Singleton
    static Siren& getInstance();
	
    // Méthodes d'initialisation et de contrôle
    void init(Log& logInstance);
    void handleStart();
    void handleStop();
    void handleStopInterrupt();

    // Méthodes pour jouer la sirène
    void playQuickTone();

private:
    // Singleton
    Siren() = default;
    Siren(const Siren&) = delete;
    Siren& operator=(const Siren&) = delete;
    Log* myLog;

    bool playing;
    uint8_t age;

    bool isInterval;

    // Timing des intermittentTone 
    uint32_t ringStartTimestamp;

    // Méthodes privées
    void playTone();
    void start();
    void stop();
    bool hasSoundedMoreThan(uint32_t period) const;
    bool hasSoundedMoreThanXinX(uint32_t duration, uint32_t durationRef) const;
};

// Fonction de calcul pour utile pour hasSoundedMoreThan()
int positiveModulo(int value, int modulus);

#endif // SIREN_H
