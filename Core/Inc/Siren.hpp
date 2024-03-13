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
    void init(Log& log);
    void handleStart();
    void handleStop();

    // Méthodes pour jouer la sirène
    void playIntermittentTone(uint32_t duration);
    void playTone(uint32_t duration);
    void playQuickTone();
    bool isPlaying() const;

private:
    // Singleton
    Siren() = default;
    Siren(const Siren&) = delete;
    Siren& operator=(const Siren&) = delete;

    Log* logInstance; // Pointeur vers l'instance de Log

    uint8_t age;
    bool playing;
    bool stopRequest;

    // Paramètres de configuration
    int pin;
    uint32_t freq;
    uint32_t durationMinimal;
    uint32_t durationMaximal;
    uint32_t durationMaxInDurationRef;
    uint32_t durationRef;
    bool isInterval;
    uint32_t intervalDuration;
    uint32_t minDelayBetweenTwoTriggers;

    // Timing des intermittentTone 
    uint32_t ringStartTimestamp;
    uint32_t ringStopTimestamp;

    // Méthodes privées
    void addLogEntry();
    bool hasSoundedMoreThan(uint32_t period) const;
    bool hasSoundedMoreThanXinX(uint32_t duration, uint32_t durationRef) const;
    void start();
    void stop();
};

// Fonction de calcul pour utile pour hasSoundedMoreThan()
int positiveModulo(int value, int modulus);
void set_time();
void get_time();
void dif_time();

extern Siren& mySiren;

#endif // SIREN_H
