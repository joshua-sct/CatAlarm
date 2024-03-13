#ifndef SIREN_H
#define SIREN_H

#include <stdint.h>
#include "rtc.h"
#include "Log.hpp"

// Structure des paramètres de Siren
struct SirenSettings {
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;
    unsigned long durationMin;
    unsigned long durationMax;
    unsigned long durationMaxInDurationRef;
    unsigned long durationRef;
    bool intermittentMode;
    int logSize;
};

// Classe Siren
class Siren {
public:
    // Singleton
    static Siren& getInstance();
	
    // Méthodes d'initialisation et de contrôle
    void init();
    void handleStart();
    void handleStop();

    // Méthodes pour jouer la sirène
    void playIntermittentTone(unsigned long duration);
    void playTone(unsigned long duration);
    void playQuickTone();
    bool isPlaying() const;

private:
    // Singleton
    Siren() {}
    Siren(const Siren&) = delete;
    Siren& operator=(const Siren&) = delete;

    Log* ptrLog;

    uint8_t age;
    bool playing;
    bool stopRequest;

    // Paramètres de configuration
    int pin;
    unsigned long freq;
    unsigned long durationMinimal;
    unsigned long durationMaximal;
    unsigned long durationMaxInDurationRef;
    unsigned long durationRef;
    bool isInterval;
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;

    // Timing des intermittentTone 
    uint32_t ringStartTimestamp;
    uint32_t ringStopTimestamp;

    // Méthodes privées
    void addLogEntry();
    bool hasSoundedMoreThan(unsigned long period) const;
    bool hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const;
    void start();
    void stop();
};

// Déclaration externe de mySiren
extern Siren mySiren;

// Fonction de calcul pour utile pour hasSoundedMoreThan()
int positiveModulo(int value, int modulus);
void set_time();
void get_time();
void dif_time();

#endif // SIREN_H
