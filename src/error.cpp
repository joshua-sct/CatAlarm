// Inclusions des bibliothèques nécessaires
#include <Arduino.h>
#include "globals.h"
#include "error.h"
#include "Siren.h"
#include "sigfox.h"

// Gère l'erreur survenue
void handleError() {
    // Erreur pour Sigfox
    if (isError(maskSigfox)) {
        sendSigfoxAlert(ERROR_CODE);
    } 


    // Erreur car la sirène a trop sonné
    if (isError(errorSirenHasBeenPlayingForTooLong)) {
        sendSigfoxAlert(ERROR_CODE);
    }
    
    // Erreur pour la Siren
    else if (isError(maskSiren)) {
        mySiren.handlePlay();
        sendSigfoxAlert(ERROR_CODE);
    }
}

// Détermine s'il y a une erreur correspondant à 'error'
bool isError(Error error) {
    return (ERROR_CODE & error);
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