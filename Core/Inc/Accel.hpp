#ifndef ACCEL_HPP
#define ACCEL_HPP

#include "Siren.hpp"       // Module Siren
#include "i2c.h"           // HAL I2C

#define RAD_TO_DEG 57.295779513082320876798154814105

class Accel {
public:
    Accel(I2C_HandleTypeDef *I2Cx, uint16_t deviceAddress);
	uint8_t init();
    void detectAbnormal(Siren& mySiren);
    void calibrate();

private:
    // Communication I2C
    I2C_HandleTypeDef *I2Cx;
    uint16_t deviceAddress;
	uint8_t Rec_Data[14];
    HAL_StatusTypeDef readI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size);
    HAL_StatusTypeDef writeI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size);

	// Valeurs mesurées
    double Ax;
    double Ay;
    double Az;
	void readAccel();

    // Valeurs calculées
    float refAx;
    float refAy;
    float refAz;

    float Acc;
    float refAcc;

    float angle;
    float refAngle;
    float angleBetweenVectors(float x1, float y1, float z1, float x2, float y2, float z2);

    float meanAx;
    float meanAy;
    float meanAz;

    bool detec;
    uint16_t ite;

//    bool hot;
//	bool inte;

    // Paramètres
    uint16_t Ndetection;
    uint16_t Ncalibration;
};

#endif // ACCEL_HPP
