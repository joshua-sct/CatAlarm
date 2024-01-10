#ifndef SIGFOX_H
#define SIGFOX_H

void sendSigfoxData(void *data);
void sendSigfoxAlert(int32_t errorCode, float latitude, float longitude);

#endif // SIGFOX_H