#include "main.h"           // Main C
#include "alt_main.h"       // Main C++
#include "tim.h"			// HAL Timer 
#include "gpio.h"			// HAL GPIO
#include "rtc.h"			// HAL RTC
#include "i2c.h"            // HAL I2C
#include "global.h"         // Paramètres et définitions générales
#include "Accel.hpp"		// Module Accel
#include "Siren.hpp"        // Module Siren
#include "Log.hpp"			// Module Log

// Définition des singletons
Log& myLog = Log::getInstance();
Siren& mySiren = Siren::getInstance();

// Déclaration des instances
Accel AccelINT(ACCEL_INTERN_I2C, ACCEL_INTERN_I2C_ADD);
Accel AccelOUT(ACCEL_EXTERN_I2C, ACCEL_EXTERN_I2C_ADD);

// Déclarations des variables
volatile bool blinkerInterruptFlag = false;
volatile bool hot;
bool detectOn = false;
bool calibrating = false;

// Debug
uint16_t countMain =0;
uint16_t countBlinker =0;
uint16_t countHot =0;

// Timer principal @100Hz
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim == &htim3) {
		// Debug
		countMain++;

		// Interrupt Blinker
		if (blinkerInterruptFlag && !hot) {
			// Désactiver le timer pendant la calibration
            HAL_TIM_Base_Stop(&htim3);
			AccelOUT.calibrate();
            HAL_TIM_Base_Start(&htim3);

			blinkerInterruptFlag = false;
			detectOn = true;
		}

		// Détection
		else if (detectOn) {
			AccelOUT.detectAbnormal(mySiren);
		}
	}
}

// Callback pour ACC et BLK
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	// Stopper la sirène
	mySiren.handleStopInterrupt();

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
    	mySiren.handleStop();
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


	myLog.init();
	mySiren.init(myLog);

	// Accel init
	AccelOUT.init();
	AccelOUT.calibrate();

	// Démarrage du timer principal
	HAL_TIM_Base_Start_IT(&htim3);

	while(1) {}
	return 0;
}
