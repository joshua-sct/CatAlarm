// Inclusions des bibliothèques nécessaires
#include <Arduino.h>            // Framework Arduino
#include <SoftwareSerial.h>     // Port série logiciel (=> sigfox)
#include "globals.h"            // Paramètres globaux
#include "error.h"              // Gestion d'erreur

// Timing de dernier envoi Sigfox
unsigned long startSigfox = 0;

// Affiche 'data' (int_32t/float) sur le port série
// Utilisation : sendSigfoxData(&myData, sizeof(myData));
void sendSigfoxData(void *data, size_t dataSize) {
    // Convertion (int_32t/float) en hexa  
    byte *b = (byte *)data;

    // Affichage sur le port série
    for (size_t i = 0; i < dataSize; ++i) {
        if (b[i] < 16) Serial.print('0');
        Serial.print(b[i], HEX);
    }
}

// Envoie d'alerte Sigfox (data = error) 
void sendSigfoxAlert(uint32_t error) {
    if (millis() - startSigfox > DELAY_SIGFOX) {
        // Construction du message Sigfox
        Serial.print("AT$SF=");
        sendSigfoxData(&error, sizeof(error));
        Serial.write('\r');     // Fin de commande pour Sigfox
        
        // Enregistrer le timing de dernier envoi Sigfox
        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox
    }
}

// Envoie d'alerte Sigfox (data = error, latitude, longitude)
void sendSigfoxAlert(uint32_t error, float latitude, float longitude) {
    if (millis() - startSigfox > DELAY_SIGFOX) {
        // Construction du message Sigfox
        Serial.print("AT$SF=");
        sendSigfoxData(&ERROR_CODE, sizeof(ERROR_CODE));
        sendSigfoxData(&latitude, sizeof(float));
        sendSigfoxData(&longitude, sizeof(float));
        Serial.write('\r');     // Fin de commande pour Sigfox

        // Enregistrer le timing de dernier envoi Sigfox
        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox
    }
}
