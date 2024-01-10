#ifndef SIREN_H
#define SIREN_H

#include <Arduino.h>

struct SirenSettings {
    int pin;
    unsigned long freq;
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;
    unsigned long durationMin;
    unsigned long durationMax;
    unsigned long durationMaxInDurationRef;
    unsigned long durationRef;
    int logSize;
};

class Siren {
public:
    Siren(const SirenSettings& params);    
    ~Siren();
    void handlePlay();
    void playIntermittentTone();
    void playQuickTone();
    private:
    struct SirenLog {
        unsigned long startTime;
        unsigned long endTime;
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
    unsigned long durationMaxInDurationRef;
    unsigned long durationRef;
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;
    
    unsigned long intermittentToneStartTime;
    unsigned long intermittentToneEndTime;

    bool isLastLogEmpty() const;
    bool hasSoundedMoreThan(unsigned long period) const;
    bool hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const;
};

extern Siren mySiren;

#endif // SIREN_H