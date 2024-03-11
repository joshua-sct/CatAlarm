#ifndef SIGFOX_H
#define SIGFOX_H

// Fonction d'envoi Sigfox
void sendSigfoxData(void *data);
void sendSigfoxAlert(uint32_t error);
void sendSigfoxAlert(uint32_t error, float latitude, float longitude);

#endif // SIGFOX_H