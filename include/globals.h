#ifndef GLOBAL_H
#define GLOBAL_H

/* Valeur max */
#define ULONG_MAX 0xFFFFFFFFUL          // 2^32 = 16^8

/* Paramètres de la sirène */
#define SIREN_PIN 6 // ou 5                             // Pin de commande de la sirène
#define SIREN_FREQ 400000                               // Fréquence du bip de la sirène
#define SIREN_INTERVAL_DURATION 200                     // Durée du bip de la sonnerie intermittente
#define SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS 1000       // Delai minimale entre le déclenchement de 2 sirènes
#define SIREN_DURATION_MIN 10000                        // Durée min d'une sirène
#define SIREN_DURATION_MAX 300000                       // Durée max d'une sirene
#define SIREN_DURATION_MAX_IN_DURATION_REF 300000       // Durée cumulée max pendant laquelle la sirène peut sonnée pendant une durée de référence 
#define SIREN_DURATION_REF 1800000                      // Durée de référence
#define SIREN_LOG_SIZE 100                              // Taille des logs des triggers de la sirène

#endif // GLOBAL_H