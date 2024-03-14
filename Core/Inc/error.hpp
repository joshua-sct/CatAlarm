#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

// Code d'erreur
extern uint32_t ERROR_CODE;

// Enumération des erreurs
enum Error : uint32_t {
    // Erreur sur 1 bit
    errorMPU                                = 1U << 15, // bit 15
    errorGyro                               = 1U << 14, // bit 14
    errorAccel                              = 1U << 13, // bit 13
    errorSirenHasBeenPlayingForTooLong      = 1U << 12, // bit 12
    errorRTC                                = 1U << 11, // bit 11

    // Masque de plusieurs erreurs
    maskSiren           = (1U << 10) - 1,   // les 10 premiers bits déclenche la sirène
    maskSigfox          = (1U << 5) - 1,    // les 5 premiers bits déclenche sigfox
};


// Fonctions de gestion d'erreur
void handleError();
bool isError(Error error);
void setError(Error error, bool isTrue);

#endif // ERROR_H
