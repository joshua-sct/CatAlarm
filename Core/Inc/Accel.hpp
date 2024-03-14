#ifndef ACCEL_HPP
#define ACCEL_HPP

#include <stdint.h>
#include "stm32g0xx_hal.h"
#include "Siren.hpp"
#include "i2c.h"

#define RAD_TO_DEG 57.295779513082320876798154814105

class Accel {
public:
    Accel(I2C_HandleTypeDef *I2Cx, uint16_t deviceAddress);
	uint8_t init();
	void readAccel();
	void readAll();
    void detectAbnormal(Siren& mySiren);
    void calibrate();
    float angleBetweenVectors(float x1, float y1, float z1, float x2, float y2, float z2);
    float angle;

private:
    I2C_HandleTypeDef *I2Cx;
    uint16_t deviceAddress;
	uint8_t Rec_Data[14];

	uint32_t timer;
	uint32_t currentTick;
	uint32_t lastTick;
	double dt;

    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;
    float Acc;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;

    float Temperature;

    float refAx;
    float refAy;
    float refAz;
    float refAcc;

    float refGx;
    float refGy;
    float refGz;
    float refAngle;


    int16_t ires;
    float Gres[100];

    float Ares;
    float refAres;

    float Axref;
    float Ayref;
    float Azref;

    float Axres;
    float Ayres;
    float Azres;

    uint8_t Ndetection;
    uint16_t Ncalibration;

    HAL_StatusTypeDef readI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size);
    HAL_StatusTypeDef writeI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size);
};

#endif // ACCEL_HPP
