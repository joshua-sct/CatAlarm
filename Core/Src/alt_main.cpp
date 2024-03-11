#include <alt_main.h>
#include <Accel.hpp>
#include "i2c.h"
#include "tim.h"
#include "main.h"
#include "gpio.h"
#include "lis2dw12_reg.h"
#include "rtc.h"
#include "Siren.hpp"
#include "Log.hpp"
//#include "global.h"

Log Log;
Accel Accel;
Siren Siren(&Log);

bool calibrating = false;
bool detectOn = false;
uint16_t countMain =0;

volatile bool blinkerInterruptFlag = false;
uint16_t countBlinker =0;

volatile bool hot;
uint16_t countHot =0;

void abnormalStart() {
	Siren.handleStart();
}

void abnormalStop() {
	Siren.handleStop();
}


// Buzzer timer 100Hz (10ms)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim == &htim3) {

		// MAIN LOOP
		countMain++;

		// Interrupt Blinker
		if (blinkerInterruptFlag && !hot) {
			calibrating = true; //debug
			//Accel.calibrate(&hi2c1);
			Accel.calibrate(&hi2c1);
			calibrating = false; //debug
			blinkerInterruptFlag = false;
			detectOn = true;
		}

		else if (detectOn) {
			Accel.detectAbnormal(&hi2c1);
		}

		// Sonner 1 seconde
		if (Siren.isPlaying()) {
			Siren.handleStop();
		}
	}
}

// Interrupt HOT / BLINKER
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	Siren.handleStop();

	// Stop (buzzer + calibrate) request
    if (GPIO_Pin == BLK_Pin) {
    	if (!hot && !blinkerInterruptFlag) {
    		// Vehicule éteint et prêt
			blinkerInterruptFlag = true;
			detectOn = true;
			countBlinker++;
		}
	}
    if (GPIO_Pin == ACC_Pin) {
    	// Vehicule s'allume
    	hot = true;
    	detectOn = false;

    	// Si interrupt blinker appelée juste avant
    	blinkerInterruptFlag = false;
    	calibrating = false;

		countHot++;
	}
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == ACC_Pin) {
    	// Véhicule s'éteint
    	hot = false;
    	Siren.handleStop();
    }
}

// Disable interrupt
void disableHotBlinkerInterrupt() {
    HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
}

// Enable interrupt
void enableHotBlinkerInterrupt() {
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

int alt_main()
{
	// VAR Init
	if (HAL_GPIO_ReadPin(ACC_GPIO_Port, ACC_Pin) == GPIO_PIN_SET) {
		hot = false;
	} else {
		hot = true;
	}

	// ***POUR LES TESTS***
	blinkerInterruptFlag = true;
	hot = false;

	Log.init();

	// Accel init
	// HAL_Delay(100);
	Accel.init(&hi2c1);
	Accel.calibrate(&hi2c1);

	//// Start 'loop' timer
	////HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	////TIM1->CCR2 = 0;
	HAL_TIM_Base_Start_IT(&htim3);
	while(1) {}
	return 0;
}

