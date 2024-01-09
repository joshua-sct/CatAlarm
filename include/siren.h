#ifndef SIREN_H
#define SIREN_H

#include <Arduino.h>

class Siren {
public:
    Siren(int pin, unsigned long freq, unsigned long durationMinimal, unsigned long durationMaximal, unsigned long intervalDuration, unsigned long minDelayBetweenTwoTriggers, unsigned long sirenHasBeenPlayingForTooLongMinimumDelayWithoutError, int logSize);    
    ~Siren();
    void handlePlay();
    void playIntermittentTone();
    void playQuickTone();
    unsigned long sirenHasBeenPlayingForTooLongLastErrorTime;
    unsigned long sirenHasBeenPlayingForTooLongMinimumDelayWithoutError;
private:
    struct SirenLog {
        unsigned long startTime;
        unsigned long duration;
    };
    //    std::list<SirenTrigger> sirenTriggers; // pas de lib c++ standard sur arduino => Pas de liste chaînés 
    const int logSize;
    int logIndex; // Indice pour suivre la position actuelle du log circulaire
    SirenLog* logs; // Utilisez un pointeur au lieu d'un tableau fixe
    void recordSirenTrigger(unsigned long startTime, unsigned long duration);

    int pin;
    unsigned long freq;
    unsigned long durationMinimal;
    unsigned long durationMaximal;
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;
    
    bool firstRing;
    bool toneState;
    unsigned long currentTime;
    unsigned long intermittentToneStartTime;
    unsigned long eachIntervalStartTime;
    unsigned long intermittentToneEndTime;
};

extern Siren mySiren;

#endif // SIREN_H