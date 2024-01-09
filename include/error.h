#ifndef ERROR_H
#define ERROR_H

#include <Arduino.h>
#include <stdint.h>

// Code d'erreur
extern uint32_t ERROR_CODE;

// Erreurs et niveau d'erreurs 
enum Error : uint32_t {
    // Erreurs (bit 0 -> 31)
    errorMPU                                = 1U << 15, // bit 15
    errorGyro                               = 1U << 14, // bit 14
    errorAccel                              = 1U << 13, // bit 13
    errorSirenHasBeenPlayingForTooLong      = 1U << 12, // bit 12


    // Masque d'erreurs
    maskSiren           = (1U << 10) - 1,   // les 10 premiers bits déclenche la sirène
    maskSigfox          = (1U << 5) - 1,    // les 5 premiers bits déclenche sigfox
};

void handleError();
void setError(Error error, bool isTrue);

#endif // ERROR_H