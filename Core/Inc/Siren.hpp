#ifndef SIREN_H
#define SIREN_H

// Structure des paramètres de Siren
struct SirenSettings {
//    int pin;
//    unsigned long freq;
    unsigned long intervalDuration;
    unsigned long minDelayBetweenTwoTriggers;
    unsigned long durationMin;
    unsigned long durationMax;
    unsigned long durationMaxInDurationRef;
    unsigned long durationRef;
    bool isInterval;
    int logSize;
};

// Classe Siren
class Siren {
public:
    // Constructeur et destructeur
    Siren(const SirenSettings& params);
    ~Siren();

    // Méthodes d'initialisation et de contrôle
    void init();
    void handlePlay();

    // Méthodes pour jouer la sirène
    void playIntermittentTone(unsigned long duration);
    void playQuickTone();

private:
    struct SirenLog {
        unsigned long startTime;
        unsigned long endTime;
    };

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

    // Log circulaire
    const int logSize;
    int logIndex; // Indice pour suivre la position actuelle du log circulaire
    SirenLog* logs; // Utilisez un pointeur au lieu d'un tableau fixe

    // Timing des intermittentTone 
    unsigned long intermittentToneStartTime;
    unsigned long intermittentToneEndTime;

    // Méthodes privées
    void recordSirenTrigger(unsigned long startTime, unsigned long duration);
    bool isLastLogEmpty() const;
    bool hasSoundedMoreThan(unsigned long period) const;
    bool hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const;
};

// Déclaration externe de mySiren
extern Siren mySiren;

// Fonction de calcul pour utile pour hasSoundedMoreThan()
int positiveModulo(int value, int modulus);

#endif // SIREN_H
