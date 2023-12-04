// Le reste du code devrait inclure les fonctions pour communiquer avec le module Sigfox
// et pour déclencher les actions appropriées en cas d'erreur ou d'alerte.

// Faire un timer pour handleSiren

// Inclusions des bibliothèques nécessaires
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Définitions des constantes
#define TOLERANCE_ANGLE_CHANGE 15    // Tolérance de changement d'angle après calibration
#define TOLERANCE_VIBRATION 0.1      // Seuil de détection de vibration (probablement en radians)
#define DELAY_SIGFOX 60000           // Délai entre les alertes Sigfox en millisecondes
#define DURATION_CALIB_GYRO 200     // Durée de calibration du gyroscope en millisecondes
#define DURATION_SIREN 10           // Durée de la sirène en millisecondes
#define SIREN_PIN 6                  // Pin de commande de la sirène
#define OUT_PIN 5                  // Pin de commande de la sirène
#define DELAY_SIREN 5               // Délai entre les activations de la sirène
#define BLINKER_PIN 1                // Pin pour la détection de clignotement (recalibration)

// Niveaux d'erreur pour déclencher les différentes alertes
int32_t errorCode = 0;
int32_t sigfoxErrorLevelTrigger = 32;    // Niveau d'erreur pour l'envoi d'un message Sigfox
int32_t sirenErrorLevelTrigger = 1024;   // Niveau d'erreur pour activer la sirène

// Configuration initiale du MPU6050
Adafruit_MPU6050 mpu;
double refXminGyro, refXmaxGyro, refYminGyro, refYmaxGyro, refZminGyro, refZmaxGyro;
double refAccel;
double toleranceAngle = TOLERANCE_ANGLE_CHANGE * (PI / 180.0);
unsigned long durationCalibration = DURATION_CALIB_GYRO;
sensors_event_t eventGyro, eventAccel, eventTemp;

// Variables de gestion du temps
unsigned long startSigfox = 0;
unsigned long delaySigfox = DELAY_SIGFOX;
unsigned long startSiren = 0;
unsigned long durationSiren = DURATION_SIREN;
unsigned long delaySiren = DELAY_SIREN;

// Configuration de la sirène
int siren = SIREN_PIN;
double sirenFreq = 400000;

// Configuration de la sortie
int out = OUT_PIN;

// Déclarations des fonctions
void calibrateGyro();
void detectGyroMovement();
void gyroGraph();

void setupMPUTiltDetection();
void verifyMPU();
void calibrateAccel();

void handleSiren();

void sendSigfoxAlert();
void sendSigfoxAlert(int32_t errorCode, float latitude, float longitude);


// Fonction de démarrage exécutée une fois au démarrage du microcontrôleur
void setup() {
    // Initialisation du port série pour le débogage
    Serial.begin(9600);

    // Configuration de la sirène
    pinMode(siren, OUTPUT);
    digitalWrite(siren, LOW);
    
    // 1st setup tone
    tone(siren, sirenFreq, durationSiren);
    
    // Initialisation et calibration des capteurs
    verifyMPU();
    setupMPUTiltDetection();
    calibrateAccel();
    calibrateGyro();

    // 2nd setup tone
    tone(siren, sirenFreq, durationSiren);
}

// Boucle principale du programme
void loop() {
    errorCode = 0; // Réinitialisation du code d'erreur

    // Processus de recalibration si nécessaire
    /*
    if (digitalRead(BLINKER_PIN) == HIGH) {
        delay(30000); // Attente avant la recalibration
        calibrateGyro();
    }*/

    // Vérification et détection des mouvements
    verifyMPU();
    detectGyroMovement();

    // Gestion des alertes
    handleSiren();
    sendSigfoxAlert();

    // Affichage de données pour le débogage (à commenter en production)
    // gyroGraph();

    errorCode = 0; // Réinitialisation du code d'erreur en fin de boucle
}

// Ajouter ici le reste des fonctions définies plus haut, avec des commentaires expliquant leur fonctionnement
// Calibre l'orientation pendant les premières secondes définies par durationCalibration
void calibrateGyro() {
  // Nettoyage initial des événements
  mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
  delay(100); // Délai pour stabiliser le capteur

  // Initialisation des valeurs de référence /#
  refXminGyro = eventGyro.gyro.x;
  refXmaxGyro = refXminGyro;
  refYminGyro = eventGyro.gyro.y;
  refYmaxGyro = refYminGyro;
  refZminGyro = eventGyro.gyro.z;
  refZmaxGyro = refZminGyro;

  unsigned long startTime = millis();
  while (millis() - startTime < durationCalibration) {
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
    delay(1); // Délai entre les lectures pour la stabilité

    // Enregistrement des valeurs extrêmes pour chaque axe
    refXminGyro = min(refXminGyro, eventGyro.gyro.x);
    refXmaxGyro = max(refXmaxGyro, eventGyro.gyro.x);
    refYminGyro = min(refYminGyro, eventGyro.gyro.y);
    refYmaxGyro = max(refYmaxGyro, eventGyro.gyro.y);
    refZminGyro = min(refZminGyro, eventGyro.gyro.z);
    refZmaxGyro = max(refZmaxGyro, eventGyro.gyro.z);
  }
}

// Calibre l'accélération (non utilisée dans le code original)
void calibrateAccel() {
  mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);
  refAccel = abs(eventAccel.acceleration.x) + abs(eventAccel.acceleration.y) + abs(eventAccel.acceleration.z);
}

// Vérifie la connexion avec le MPU6050 et initialise si nécessaire
void verifyMPU() {
  if (!mpu.begin()) {
    errorCode |= (1 << 15); // Bit 15 est défini pour une défaillance du MPU6050
  }
}

// Configuration de la détection de changement d'angle
void setupMPUTiltDetection() {
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

// Détecte les mouvements à l'aide du gyroscope
void detectGyroMovement() {
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

    // Détection de changement d'angle pour chaque axe
    if (eventGyro.gyro.x > refXmaxGyro + toleranceAngle || eventGyro.gyro.x < refXminGyro - toleranceAngle) {
        errorCode |= (1 << 14); // Bit 14 est défini pour un changement d'angle
    }
    if (eventGyro.gyro.y > refYmaxGyro + toleranceAngle || eventGyro.gyro.y < refYminGyro - toleranceAngle) {
        errorCode |= (1 << 14);
    }
    if (eventGyro.gyro.z > refZmaxGyro + toleranceAngle || eventGyro.gyro.z < refZminGyro - toleranceAngle) {
        errorCode |= (1 << 14);
    }

    // Détection de vibration (utilisation de l'accéléromètre)
    double vibration = abs(eventAccel.acceleration.x) + abs(eventAccel.acceleration.y) + abs(eventAccel.acceleration.z);
    if (vibration > refAccel + TOLERANCE_VIBRATION || vibration < refAccel - TOLERANCE_VIBRATION) {
        errorCode |= (1 << 13); // Bit 13 est défini pour la détection d'accélération
    }
    refAccel = vibration; // Mise à jour de la valeur de référence pour les vibrations
}

// Gestion de la sirène
void handleSiren() {
    unsigned long currentMillis = millis();

    // Arrêt de la sirène après la durée définie
    if (currentMillis - startSiren > durationSiren) {
        noTone(siren);
    }

    // Déclenchement de la sirène si l'erreur est suffisamment importante
    if (errorCode >= sirenErrorLevelTrigger) {
        // Sonnerie intermittente
        if (currentMillis - startSiren > delaySiren) {
            startSiren = currentMillis;
            tone(siren, sirenFreq, durationSiren);
            tone(out, sirenFreq, durationSiren);
        }
    }
}

// Convertit une donnée (int_32t/float) en représentation hexadécimale et l'affiche sur la sortie série
void sendSigfoxData(void *data) {
    byte *b = (byte *)data;
    size_t size = sizeof(*data);
    for (size_t i = 0; i < size; ++i) {
        if (b[i] < 16) Serial.print('0');
        Serial.print(b[i], HEX);
    }
}

// Envoie une alerte via Sigfox selon l'erreur
void sendSigfoxAlert() {
    if (errorCode >= sigfoxErrorLevelTrigger) {
        if (millis() - startSigfox > delaySigfox) {
        startSigfox = millis(); // Début de la pause entre les envois de messages Sigfox
        // Fonction d'envoi du message Sigfox à implémenter
        }
    }
}

// Envoie une alerte via Sigfox incluant le code d'erreur et les coordonnées GPS
void sendSigfoxAlert(int32_t errorCode, float latitude, float longitude) {
  // Configuration des pins RX et TX si nécessaire
  // SoftwareSerial sigfoxSerial(serial_fox_rx, serial_fox_tx);
  // sigfoxSerial.begin(9600);

  // Construction du message Sigfox
  Serial.print("AT$SF=");
  sendSigfoxData(&errorCode);
  sendSigfoxData(&latitude);
  sendSigfoxData(&longitude);
  Serial.write('\r'); // Fin de commande pour Sigfox

  // Si utilisation de SoftwareSerial
  // while(sigfoxSerial.available()) {
  //    Serial.write(sigfoxSerial.read());
  // }
}

// Affiche les données du gyroscope et de l'accéléromètre
void gyroGraph() {
    mpu.getEvent(&eventGyro, &eventAccel, &eventTemp);

    // Gyroscope : X,Y,Z avec seuils de tolérance
    Serial.print("\nGX: "); Serial.print(eventGyro.gyro.x);
    Serial.print(" Tol+: "); Serial.print(refXmaxGyro + toleranceAngle);
    Serial.print(" Tol-: "); Serial.print(refXminGyro - toleranceAngle);

    Serial.print(" GY: "); Serial.print(eventGyro.gyro.y);
    Serial.print(" Tol+: "); Serial.print(refYmaxGyro + toleranceAngle);
    Serial.print(" Tol-: "); Serial.print(refYminGyro - toleranceAngle);

    Serial.print(" GZ: "); Serial.print(eventGyro.gyro.z);
    Serial.print(" Tol+: "); Serial.print(refZmaxGyro + toleranceAngle);
    Serial.print(" Tol-: "); Serial.println(refZminGyro - toleranceAngle);

    // Accéléromètre :  avec seuils de tolérance
    Serial.print("Vibration Ref: "); Serial.print(refAccel);
    Serial.print(" Tol+: "); Serial.print(refAccel + TOLERANCE_VIBRATION);
    Serial.print(" Tol-: "); Serial.println(refAccel - TOLERANCE_VIBRATION);
}