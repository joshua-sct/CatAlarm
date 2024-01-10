#include <Arduino.h>
#include "error.h"
#include "Siren.h"
#include "globals.h"

// Code d'erreur
uint32_t ERROR_CODE = 0;             // Définition de la variable externe
uint32_t ERROR_CODE_PERSISTENT = 0;  // Copie du code d'erreur en cas d'alarme persistente

bool sirenState;

// Fonction pour tester un bit à une position spécifique
//bool testBit(unsigned long value, int bitPosition) {
//    return (value >> bitPosition) & 1;
//}

// bool getError(error)

// Gère l'erreur survenue
void handleError() {
    // Erreur à envoyer à Sigfox
    if (ERROR_CODE & maskSigfox) {
        // sigfox_send()
    } 


    if (ERROR_CODE & maskSigfox) {
        mySiren.handlePlay();
    }

    /* Sans utilisation des logs pour la sirene pour savoir si SirenHasBeenPlayingForTooLong */
    // // La sirène a sonné trop longtemps
    // if (ERROR_CODE & errorSirenHasBeenPlayingForTooLong) {
    //     unsigned long currentTime = millis();

    //     /* On distingue 2 cas : erreur maskSiren, ou pas d'erreur maskSiren */  

    //     // Erreur sirène
    //     if (ERROR_CODE & maskSiren) {
    //         // Garder la trace de la dernière erreure liée à la sirène
    //         mySiren.sirenHasBeenPlayingForTooLongLastErrorTime = currentTime;
    //     }
    //     // Pas d'erreur sirène, et plus depuis un certain temps
    //     else if (mySiren.sirenHasBeenPlayingForTooLongLastErrorTime - currentTime > mySiren.sirenHasBeenPlayingForTooLongMinimumDelayWithoutError) {
    //         setError(errorSirenHasBeenPlayingForTooLong, false);
    //         // L'erreur 
    //     }
    // }
    // // Une erreur est liée à la sirène et il n'y a pas errorSirenHasBeenPlayingForTooLong
    // else if ((ERROR_CODE & maskSiren)) { // Erreur du gyroscope ou de l'accéléromètre
    //     mySiren.handlePlay();
    // }

}


// Actualise le code d'erreur et gère l'erreur si besoin 
void setError(Error error, bool isTrue) {
    if (isTrue) {
        ERROR_CODE |= static_cast<uint32_t>(error); // bit correspondant à erreur e mis à 1 ("OU" avec le mask)
        handleError();
    } else {
        ERROR_CODE &= ~static_cast<uint32_t>(error); // bit correspondant à erreur e mis à 0 ("ET" avec le mask inverse)
    }
}