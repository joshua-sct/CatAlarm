#include <Arduino.h>
#include "error.h"
#include "Siren.h"
#include "globals.h"

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


    if (ERROR_CODE & maskSiren) {
        mySiren.handlePlay();
    }

    // La sirène a sonné trop longtemps
    if (ERROR_CODE & errorSirenHasBeenPlayingForTooLong) {
        // sifgox_send()
    }
   

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