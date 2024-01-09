#include <Arduino.h>
#include <SoftwareSerial.h>
#include "error.h"

#define DELAY_SIGFOX 60000           // Délai entre les alertes Sigfox en millisecondes

// Variables de gestion du temps
unsigned long startSigfox = 0;
unsigned long delaySigfox = DELAY_SIGFOX;


// Convertit une donnée (int_32t/float) en représentation hexadécimale et l'affiche sur la sortie série
// Utilisation : sendSigfoxData(&myData, sizeof(myData));
void sendSigfoxData(void *data, size_t dataSize) {
    byte *b = (byte *)data;
    for (size_t i = 0; i < dataSize; ++i) {
        if (b[i] < 16) Serial.print('0');
        Serial.print(b[i], HEX);
    }
}

// Envoie une alerte via Sigfox incluant le code d'erreur et les coordonnées GPS
// Si l'erreur est suffisement importante
void sendSigfoxAlert(float latitude, float longitude) {
    // Configuration des pins RX et TX si nécessaire
    // SoftwareSerial sigfoxSerial(serial_fox_rx, serial_fox_tx);
    // sigfoxSerial.begin(9600);

    // Si (Erreur suffisement importante) ET (delaySigfox minimum entre 2 alertes sigfox)
    
    // TODO
    //if (ERROR_CODE >= sigfoxErrorLevelTrigger && millis() - startSigfox > delaySigfox) {
        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox

        // Construction du message Sigfox
        Serial.print("AT$SF=");
        sendSigfoxData(&ERROR_CODE, sizeof(ERROR_CODE));
        sendSigfoxData(&latitude, sizeof(float));
        sendSigfoxData(&longitude, sizeof(float));
        Serial.write('\r'); // Fin de commande pour Sigfox

    //}
    
    // Si utilisation de SoftwareSerial 
    // while(sigfoxSerial.available()) {
    //    Serial.write(sigfoxSerial.read());
    // }
}