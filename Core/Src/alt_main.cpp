#include <alt_main.h>
#include <Accel.hpp>
#include "i2c.h"
#include "tim.h"
#include "main.h"
#include "gpio.h"
#include "lis2dw12_reg.h"
//#include "global.h"

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

#include "rtc.h"

void set_time (void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  // if not (GPS time) => 0
  sTime.Hours = 0x0; // set hours
  sTime.Minutes = 0x0; // set minutes
  sTime.Seconds = 0x0; // set seconds
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    //err
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x0; // date
  sDate.Year = 0x0; // year
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
	//err
  }
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}


RTC_TimeTypeDef initTime;
RTC_DateTypeDef initDate;

RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

void get_time(void)
{
/* Get the RTC current Time */
 HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
/* Get the RTC current Date */
 HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
///* Display time Format: hh:mm:ss */
// sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
///* Display date Format: dd-mm-yy */
// sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);
}

int32_t differenceSeconds;

void diff_time(RTC_DateTypeDef g1Date, RTC_TimeTypeDef g1Time, RTC_DateTypeDef g2Date, RTC_TimeTypeDef g2Time)
{
	// Convertir les deux moments en secondes
	uint32_t g1Seconds = g1Time.Hours * 3600 + g1Time.Minutes * 60 + g1Time.Seconds;
	g1Seconds += (g1Date.Date - 1) * 86400; // Ajouter les secondes pour les jours écoulés

	uint32_t g2Seconds = g2Time.Hours * 3600 + g2Time.Minutes * 60 + g2Time.Seconds;
	g2Seconds += (g2Date.Date - 1) * 86400; // Ajouter les secondes pour les jours écoulés

	// Calculer la différence en secondes
	differenceSeconds = g2Seconds - g1Seconds;

	// Calculer le temps écoulé en jours, heures, minutes et secondes
	int32_t jours = differenceSeconds / 86400;
	int32_t heures = (differenceSeconds % 86400) / 3600;
	int32_t minutes = (differenceSeconds % 3600) / 60;
	int32_t secondes = differenceSeconds % 60;
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
	//TIM1->CCR2 = 700; // PWM buzzer = gentil
	//if (HAL_TIM_Base_Start_IT(&htim3) == HAL_OK) {
		//TIM1->CCR2 = 400; // PWM buzzer = gentil
	//HAL_GPIO_WritePin(UARTINFO_GPIO_Port, UARTINFO_Pin, GPIO_PIN_SET);
	if (!buzzerOn) {
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		buzzerOn = true;
		get_time();
		diff_time(initDate,initTime,gDate,gTime);
	}
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
	set_time();

	get_time();
	initTime = gTime;
	initDate = gDate;

	//// Start 'loop' timer
	////HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	////TIM1->CCR2 = 0;
	HAL_TIM_Base_Start_IT(&htim3);
	while(1) {}
	return 0;
}

