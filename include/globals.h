#ifndef GLOBAL_H
#define GLOBAL_H

/* Valeur max */
#define ULONG_MAX 0xFFFFFFFFUL          // 2^32 = 16^8

/* Paramètres de la sirène */
#define SIREN_PIN 6 // ou 5                             // Pin de commande de la sirène
#define SIREN_DURATION_MINIMAL 10000                    // Durée minimale de la sirène en millisecondes = durée d'un ring()
#define SIREN_INTERVAL_DURATION 200                     // Durée du bip de la sonnerie intermittente
#define SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS 1000    // Delai minimale entre le déclenchement de 2 sirènes
#define SIREN_FREQ 400000                               // Fréquence du bip de la sirène
#define SIREN_MAX_DURATION 300000                       // Duréee maximale de la sirene
#define SIREN_HAS_BEEN_PLAYING_FOR_TOO_LONG_MINIMUM_DELAY_WITHOUT_ERROR 60000 // Durée minimale sans erreur pour un retour à la normal quand la sirène a précédemment sonné trop longtemps
#define SIREN_LOG_SIZE 100                              // Taille des logs des triggers de la sirène
#endif // GLOBAL_H