#include "alt_main.h"      // Main C++
#include "Accel.hpp"       // Module Accel
#include "Siren.hpp"       // Module Siren
#include "global.h"        // Paramètres et définitions générales
#include "i2c.h"           // HAL I2C
#include "lis2dw12_reg.h"  // Driver LIS2DW12
#include <string.h>        // Chaînes de caractères

static int16_t data_raw_acceleration[3];

#define BOOT_TIME 50 // ms
#define TIMEOUT 100

HAL_StatusTypeDef Accel::readI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size){
	return HAL_I2C_Mem_Read(I2Cx, deviceAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, TIMEOUT);
}

HAL_StatusTypeDef Accel::writeI2C(uint16_t MemAddress, uint8_t *pData, uint16_t Size){
	return HAL_I2C_Mem_Write(I2Cx, deviceAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, TIMEOUT);
}

Accel::Accel(I2C_HandleTypeDef *I2Cx, uint16_t deviceAddress) :
	I2Cx(I2Cx),
	deviceAddress(deviceAddress)
	{}

uint8_t Accel::init()
{
	HAL_Delay(BOOT_TIME);
	HAL_StatusTypeDef ret;

	uint8_t whoamI;
	readI2C(LIS2DW12_WHO_AM_I, &whoamI, 1);
	if (whoamI != LIS2DW12_ID) {
		while (1) { }
	}
	/* Restore default configuration */
	lis2dw12_ctrl2_t reg2;
	if (readI2C(LIS2DW12_CTRL2, (uint8_t *) &reg2, 1) == HAL_OK)
	{
		reg2.soft_reset = PROPERTY_ENABLE;
		writeI2C(LIS2DW12_CTRL2, (uint8_t *) &reg2, 1);
	}

	uint8_t rst;
	do {
		readI2C(LIS2DW12_CTRL2, (uint8_t *) &reg2, 1);
		rst = reg2.soft_reset;
	} while (rst);

	/* Enable Block Data Update */
	if (readI2C(LIS2DW12_CTRL2, (uint8_t *) &reg2, 1) == HAL_OK)
	{
		reg2.bdu = PROPERTY_ENABLE;
		writeI2C(LIS2DW12_CTRL2, (uint8_t *) &reg2, 1);
	}

	/* Set full scale */
	lis2dw12_ctrl6_t reg6;
	if (readI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1) == HAL_OK)
	{
		reg6.fs = LIS2DW12_2g;
		writeI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	}

	/* Configure filtering chain
	* Accelerometer - filter path / bandwidth
	*/
	lis2dw12_ctrl_reg7_t reg7;
	ret = readI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	if (ret == HAL_OK)
	{
		reg6.fds = ((uint8_t) LIS2DW12_LPF_ON_OUT & 0x10U) >> 4;
		ret = readI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	}

	if (ret == HAL_OK)
	{
		ret = readI2C(LIS2DW12_CTRL_REG7, (uint8_t *) &reg7, 1);
	}

	if (ret == HAL_OK)
	{
		reg7.usr_off_on_out = (uint8_t) LIS2DW12_LPF_ON_OUT & 0x01U;
		ret = readI2C(LIS2DW12_CTRL_REG7, (uint8_t *) &reg7, 1);
	}


  	ret = readI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	if (ret == HAL_OK)
	{
		reg6.bw_filt = (uint8_t) LIS2DW12_ODR_DIV_4;
		ret = writeI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	}

	/* Configure power mode */

	lis2dw12_ctrl1_t reg1;
	ret = readI2C(LIS2DW12_CTRL1, (uint8_t *) &reg1, 1);
	if (ret == HAL_OK)
	{
		reg1.mode = ((uint8_t) LIS2DW12_HIGH_PERFORMANCE & 0x0CU) >> 2;
		reg1.lp_mode = (uint8_t) LIS2DW12_HIGH_PERFORMANCE & 0x03U ;
		ret = writeI2C(LIS2DW12_CTRL1, (uint8_t *) &reg1, 1);
	}

	if (ret == HAL_OK)
	{
		ret = readI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	}

	if (ret == HAL_OK)
	{
		reg6.low_noise = ((uint8_t) LIS2DW12_HIGH_PERFORMANCE & 0x10U) >> 4;
		ret = writeI2C(LIS2DW12_CTRL6, (uint8_t *) &reg6, 1);
	}

	/* Set Output Data Rate */
	lis2dw12_ctrl3_t reg3;
	ret = readI2C(LIS2DW12_CTRL1, (uint8_t *) &reg1, 1);
	if (ret == HAL_OK)
	{
		reg1.odr = (uint8_t) LIS2DW12_XL_ODR_25Hz;
		ret = writeI2C(LIS2DW12_CTRL1, (uint8_t *) &reg1, 1);
	}

	if (ret == HAL_OK)
	{
		ret = readI2C(LIS2DW12_CTRL3, (uint8_t *) &reg3, 1);
	}

	if (ret == HAL_OK)
	{
		reg3.slp_mode = ((uint8_t) LIS2DW12_XL_ODR_25Hz & 0x30U) >> 4;
		ret = writeI2C(LIS2DW12_CTRL3, (uint8_t *) &reg3, 1);
	}

    Ndetection = 32;
	Ncalibration = 500;
	refAngle = 3.5;
	return 1;
}

void Accel::readAccel()
{
	uint8_t available;

	lis2dw12_status_t status;
	readI2C(LIS2DW12_STATUS, (uint8_t *) &status, 1);
	available = status.drdy;

    if (available) {
		/* Read acceleration data */
		memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
		uint8_t buff[6];
		readI2C(LIS2DW12_OUT_X_L, buff, 6);
		data_raw_acceleration[0] = (int16_t)buff[1];
		data_raw_acceleration[0] = (data_raw_acceleration[0] * 256) + (int16_t)buff[0];
		data_raw_acceleration[1] = (int16_t)buff[3];
		data_raw_acceleration[1] = (data_raw_acceleration[1] * 256) + (int16_t)buff[2];
		data_raw_acceleration[2] = (int16_t)buff[5];
		data_raw_acceleration[2] = (data_raw_acceleration[2] * 256) + (int16_t)buff[4];

		// LSB to mg
		Ax = ((float_t)data_raw_acceleration[0]) * 0.061f;
		Ay = ((float_t)data_raw_acceleration[1]) * 0.061f;
		Az = ((float_t)data_raw_acceleration[2]) * 0.061f;
    }
}

void Accel::detectAbnormal(Siren& mySiren) {
	meanAx = 0;
	meanAy = 0;
	meanAz = 0;
	for (int i=0; i<Ndetection;i++) {
		readAccel();
		meanAx += Ax;
		meanAy += Ay;
		meanAz += Az;
	}

	meanAx /= Ndetection;
	meanAy /= Ndetection;
	meanAz /= Ndetection;

	angle = angleBetweenVectors(meanAx, meanAy, meanAz, refAx, refAy, refAz);
	Acc = sqrt(Ax*Ax + Ay*Ay + Az*Az);

//	if (angle > refAngle || abs(Acc-refAcc) > 4)
	if ((abs(Acc-refAcc) > 8) || (angle > refAngle))
	{
		mySiren.handleStart();
	} else {
		mySiren.handleStop();
	}
//	if (HAL_GPIO_ReadPin(ACC_GPIO_Port, ACC_Pin) == GPIO_PIN_SET) {
//			hot = true;
//		} else {
//			hot = false;
//		}
//	if (HAL_GPIO_ReadPin(BLK_GPIO_Port, BLK_Pin) == GPIO_PIN_SET) {
//			inte = true;
//			HAL_Delay(500);
//		} else {
//			inte = false;
//		}
}

// TODO : Mettre un timer pour qu'il execute @freq pendant @T
// Enregistrement des valeurs extrêmes pour chaque axe
void Accel::calibrate() {
	refAx = 0;
	refAy = 0;
	refAz = 0;
	refAcc = 0;

	for (int i=0; i < Ncalibration; i++) {
		readAccel();
		refAx += Ax;
		refAy += Ay;
		refAz += Az;
		Acc = sqrt(Ax*Ax+Ay*Ay+Az*Az);
//		if (refAcc < Acc) {
//			refAcc = Acc;
//		}
		refAcc += Acc;
	}
	// SEUIL FIXE
	refAcc=refAcc/Ncalibration;

	refAx /= Ncalibration;
	refAy /= Ncalibration;
	refAz /= Ncalibration;
}

float Accel::angleBetweenVectors(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float dot_product = x1*x2 + y1*y2 + z1*z2;
    float norm_product = sqrtf(x1*x1 + y1*y1 + z1*z1) * sqrtf(x2*x2 + y2*y2 + z2*z2);
    float angle = acosf(dot_product / norm_product);
    return angle * 180.0f / 3.14159265359f; // rad => deg
}
