// Inclusions des bibliothèques nécessaires
#include "global.h"            // Paramètres globaux
#include "error.hpp"              // Gestion d'erreur
#include <stdint.h>
#include <stddef.h>        // Inclusion de l'en-tête pour size_t

// Timing de dernier envoi Sigfox
uint32_t startSigfox = 0;

// Affiche 'data' (int_32t/float) sur le port série
// Utilisation : sendSigfoxData(&myData, sizeof(myData));
void sendSigfoxData(void *data, size_t dataSize) {
    // Convertion (int_32t/float) en hexa  
    uint8_t *b = (uint8_t *)data;

    // Affichage sur le port série
    for (size_t i = 0; i < dataSize; ++i) {
//        if (b[i] < 16) Serial.print('0');
//        Serial.print(b[i], HEX);
    }
}

// Envoie d'alerte Sigfox (data = error) 
void sendSigfoxAlert(uint32_t error) {
    //*
//	if (millis() - startSigfox > DELAY_SIGFOX) {
//        // Construction du message Sigfox
////        Serial.print("AT$SF=");
////        sendSigfoxData(&error, sizeof(error));
////        Serial.write('\r');     // Fin de commande pour Sigfox
//
//        // Enregistrer le timing de dernier envoi Sigfox
////        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox
//    }
}

// Envoie d'alerte Sigfox (data = error, latitude, longitude)
void sendSigfoxAlert(uint32_t, float latitude, float longitude) {
//*
//    if (millis() - startSigfox > DELAY_SIGFOX) {
//        // Construction du message Sigfox
////        Serial.print("AT$SF=");
////        sendSigfoxData(&ERROR_CODE, sizeof(ERROR_CODE));
////        sendSigfoxData(&latitude, sizeof(float));
////        sendSigfoxData(&longitude, sizeof(float));
////        Serial.write('\r');     // Fin de commande pour Sigfox
//
//        // Enregistrer le timing de dernier envoi Sigfox
////        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox
//    }
}
