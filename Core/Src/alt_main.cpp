#include "main.h"           // Main C
#include "alt_main.h"       // Main C++
#include "Accel.hpp"		// Module Accel
#include "Siren.hpp"        // Module Siren
#include "Log.hpp"			// Module Log
#include "global.h"         // Paramètres et définitions générales
#include "tim.h"			// HAL Timer 
#include "gpio.h"			// HAL GPIO
#include "i2c.h"            // HAL I2C

// Définition des singletons
Log& myLog = Log::getInstance();
Siren& mySiren = Siren::getInstance();

// Déclaration des instances
Accel AccelIN(ACCEL_IN_I2C, ACCEL_IN_I2C_ADD);
Accel AccelOUT(ACCEL_OUT_I2C, ACCEL_OUT_I2C_ADD);

// Déclarations des variables
volatile bool blinkerInterruptFlag = false;
volatile bool hot;
bool blk;

bool detectOn = false;
bool calibrating = false;
bool bump = false;

bool firstStart = true;

// Debug
uint16_t countBlinker =0;
uint16_t countHot =0;
uint16_t countMain =0;

int8_t n=0;
const int8_t hist_n = 5;
bool hist_hot[hist_n];
bool hist_blk[hist_n];

// Timer principal @100Hz
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim == &htim3) {

		/* PB CEM */
		// 1. hot front montant => blinker front montant
		// 2. blinker front montant => blinker front descendant
		// 3. detect => hot front montant

		n++; // NOTE: incrémenter n plus lentement si freq trop grande

		// Solution PB CEM : Obtenir hot & blk fiables
		if (n>hist_n) {
			n=0;
			
			// Check si 5 derniers hot sont false
			bool every_hot = true;
			for (bool elem : hist_hot) {
				if (elem) {
					every_hot = false;
					break;
				}
			}
			if (every_hot) {
				hot = false;
			} else {
				// Check si 5 derniers hot sont vrais
				bool every_hot = true;
				for (bool elem : hist_hot) {
					if (!elem) {
						every_hot = false;
						break;
					}
				}
				if (every_hot) {
					hot = true;
				}
			}

			// Check si 5 derniers blk sont false
			bool every_blk = true;
			for (bool elem : hist_blk) {
				if (elem) {
					every_blk = false;
					break;
				}
			}
			if (every_blk) {
				blk = false;
			} else {
				// Check si 5 derniers blk sont vrais
				bool every_blk = true;
				for (bool elem : hist_blk) {
					if (!elem) {
						every_blk = false;
						break;
					}
				}
				if (every_blk) {
					blk = true;
				}
			}
		}

		// Contournement CEM 3.
		hist_hot[n] = (HAL_GPIO_ReadPin(ACC_GPIO_Port, ACC_Pin) == GPIO_PIN_SET);
		hist_blk[n] = (HAL_GPIO_ReadPin(BLK_GPIO_Port, BLK_Pin) == GPIO_PIN_SET);

		if (blk) {
			firstStart = false;
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

			HAL_TIM_Base_Stop_IT(&htim3);
			AccelOUT.calibrate();
			HAL_TIM_Base_Start_IT(&htim3);

			if (!hot) {
				detectOn = true;
			}
		}

		if (hot) {
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	    	detectOn = false;
		}

		if (detectOn) {
			AccelOUT.detectAbnormal(mySiren);
		}
	}
}

// Callback pour ACC et BLK
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	// Stopper la sirène
	//mySiren.handleStopInterrupt();

	// Stop (buzzer + calibrate) request
    // if (GPIO_Pin == BLK_Pin) {
    	// if (!hot && !blinkerInterruptFlag && !bump) {
    		// Vehicule éteint et prêt
			// blinkerInterruptFlag = true;
//			detectOn = true;
//			countBlinker++;
		// }
	// }
//    if (GPIO_Pin == ACC_Pin) {
//    	// Vehicule s'allume
//    	hot = true;
//    	detectOn = false;
//
//    	// Si interrupt blinker appelée juste avant
//    	blinkerInterruptFlag = false;
//    	calibrating = false;
//
//		countHot++;
//	}
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
//    if (GPIO_Pin == ACC_Pin) {
    	// Véhicule s'éteint
//    	hot = false;
//    	mySiren.handleStopInterrupt();
//    }
}

// Disable interrupt
void disableHotBlinkerInterrupt() {
    HAL_NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
    HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
}

// Enable interrupt
void enableHotBlinkerInterrupt() {
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}

int alt_main()
{
	disableHotBlinkerInterrupt();

	// Initialisation blk & hot
	if (HAL_GPIO_ReadPin(ACC_GPIO_Port, ACC_Pin) == GPIO_PIN_SET) {
		hot = true;
		for (int i = 0; i < hist_n; ++i) {
        	hist_hot[i] = true;
    	}
	} else {
		hot = false;
		for (int i = 0; i < hist_n; ++i) {
        	hist_hot[i] = false;
    	}
	}
	for (int i = 0; i < hist_n; ++i) {
		hist_blk[i] = false;
	}


	// ***POUR LES TESTS***
	//	blinkerInterruptFlag = true;
	//	hot = false;

	firstStart = true;

	myLog.init();
	mySiren.init(myLog);

	// Accel init
	AccelOUT.init();
	AccelOUT.calibrate();

	// Démarrage du timer principal
	HAL_TIM_Base_Start_IT(&htim3);

	while(1) {
//		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
	}
	return 0;
}
