#ifndef GLOBAL_H
#define GLOBAL_H

/* Paramètres de la Sirène */
#define SIREN_PIN 6 // ou 5                             // Pin de commande de la sirène
#define SIREN_FREQ 400000                               // Fréquence du bip de la sirène
#define SIREN_INTERVAL_DURATION 200                     // Durée du bip de la sonnerie intermittente
#define SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS 1000       // Delai minimale entre le déclenchement de 2 sirènes
#define SIREN_DURATION_MIN 10000                        // Durée min d'une sirène
#define SIREN_DURATION_MAX 300000                       // Durée max d'une sirene
#define SIREN_DURATION_MAX_IN_DURATION_REF 300000       // Durée cumulée max pendant laquelle la sirène peut sonnée pendant une durée de référence 
#define SIREN_DURATION_REF 1800000                      // Durée de référence
#define SIREN_LOG_SIZE 100                              // Taille des logs des triggers de la sirène

// Paramètres de l'AccelGyro
#define ACCEL_TOLERANCE_VIBRATION 0.1                   // Seuil de détection de vibration (probablement en radians)
#define GYRO_TOLERANCE_ANGLE 15                         // Tolérance de changement d'angle après calibration
#define GYRO_DURATION_CALIB 200                         // Durée de calibration du gyroscope en millisecondes
#define BLINKER_PIN 1                                   // Pin pour la détection de clignotement (recalibration)

#endif // GLOBAL_H