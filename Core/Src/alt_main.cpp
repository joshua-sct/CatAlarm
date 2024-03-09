#include <alt_main.h>
#include <Accel.hpp>
#include "i2c.h"
#include "tim.h"
#include "main.h"
//#include "globals.hpp"
#include "gpio.h"

Accel Accel;

bool calibrating = false;
bool detectOn = false;
uint16_t countMain =0;

volatile bool blinkerInterruptFlag = false;
uint16_t countBlinker =0;

volatile bool hot;
uint16_t countHot =0;

bool buzzerOn = false;
bool buzzerOn2 = false;
bool buzzerStopRequest = false;
uint8_t buzzerCount = 0;

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
		if (buzzerOn) {
			// duréee minimale buzzer = 100*10ms = 1s
			if (buzzerStopRequest == true && (buzzerCount > 100)) {
				//TIM1->CCR2 = 0; // PWM buzzer = 0
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
				//HAL_GPIO_WritePin(UARTINFO_GPIO_Port, UARTINFO_Pin, GPIO_PIN_RESET);
				buzzerOn = false;
				buzzerStopRequest = false;
			}
			buzzerCount++;
		}
	}
}

void handleStartBuzzer() {
	buzzerOn2 = true;
	//TIM1->CCR2 = 700; // PWM buzzer = gentil
	//if (HAL_TIM_Base_Start_IT(&htim3) == HAL_OK) {
		//TIM1->CCR2 = 400; // PWM buzzer = gentil
	//HAL_GPIO_WritePin(UARTINFO_GPIO_Port, UARTINFO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	buzzerOn = true;
	buzzerCount = 0;
	buzzerStopRequest = false;
//	}
}

void handleStopBuzzer() {
	buzzerStopRequest = true;
}

// Interrupt HOT / BLINKER
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	handleStartBuzzer();

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
    	handleStopBuzzer();
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

#include "lis2dw12_reg.h"
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

