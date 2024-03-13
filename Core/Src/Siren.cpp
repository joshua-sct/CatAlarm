// Inclusions des bibliothèques nécessaires
#include <Siren.hpp>		// Gestion de Siren
#include "global.h"			// Paramètres globaux
#include "error.hpp"		// Gestion d'erreurs
#include "stm32g0xx_hal.h"
#include "alt_main.h"
#include "sigfox.hpp"
#include "gpio.h"
#include "Log.hpp"

SirenSettings mySirenSettings = {
	SIREN_DURATION_MIN,
	SIREN_DURATION_MAX,
	SIREN_DURATION_MAX_IN_DURATION_REF,
	SIREN_DURATION_REF,
	SIREN_INTERVAL_DURATION,
	SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS,
	SIREN_INTERMITTENT_MODE,
	SIREN_LOG_SIZE
};

// Singleton
Siren& Siren::getInstance() {
    static Siren instance;
    return instance;
}

// Injection de dépendances
void Siren::init(Log& log) {
    Log& logInstance = Log::getInstance();	
}

// Gère la sirène lors de son appel
void Siren::handleStart() {
	// Première sonnerie
	if (logInstance->isLastEntryEmpty()) {
		// Première sonnerie
		playIntermittentTone(durationMinimal);
	}
	
	// n-ième sonnerie
	else {
		// Si la sirène a trop sonné en continu
		if (logInstance->hasRungMoreThan(durationMaximal)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}

		// Si la sirène a trop sonné pendant une période donnée
		else if (logInstance->hasRungMoreThanXinX(durationMaxInDurationRef, durationRef)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}
		
		// Sinon la sirène doit encore sonner 
		else {
			playIntermittentTone(durationMinimal);
			sendSigfoxAlert(ERROR_CODE);
			setError(errorSirenHasBeenPlayingForTooLong, false);
		}
	}
}

void Siren::handleStop() {
	// duréee minimale buzzer = 100*10ms = 1s
	if (stopRequest == true && (age > 100)) {
		//TIM1->CCR2 = 0; // PWM buzzer = 0
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(UARTINFO_GPIO_Port, UARTINFO_Pin, GPIO_PIN_RESET);
		playing = false;
		stopRequest = false;
	}
	age++;
}

void Siren::start() {
	if (!playing) {
		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
		playing = true;
		get_time();
		//diff_time(initDate,initTime,gDate,gTime);
	}
	age = 0;
	stopRequest = false;
}

void Siren::stop() {
	stopRequest = true;
}


// Sonne de manière intermittente pendant durationMinimal
void Siren::playIntermittentTone(uint32_t duration) {
    //* intermittentToneStartTime = millis();

	//* Sonne pendant durationMinimal
//    while (millis() - intermittentToneStartTime < duration) {
//        // Jouer une période de sirène
//		handleStartBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de tonalité
//		handleStopBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de silence
//    }

	//intermittentToneEndTime = millis();

	// Enregistre l'entrée correspondante dans le journal 'logs'
	addLogEntry();
}

void Siren::playTone(uint32_t duration) {
    //* intermittentToneStartTime = millis();

	//* Sonne pendant durationMinimal
//    while (millis() - intermittentToneStartTime < duration) {
//        // Jouer une période de sirène
//		handleStartBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de tonalité
//		handleStopBuzzer();
//		HAL_Delay(intervalDuration / 2); // Temps de silence
//    }

	//intermittentToneEndTime = millis();

	// Enregistre l'entrée correspondante dans le journal 'logs'
	addLogEntry();
}

// Joue un bip de sirène
void Siren::playQuickTone() {
	// Return if ok
	handleStart();
	HAL_Delay(intervalDuration / 2); // Temps de tonalité
	handleStop();
}

// Ajoute une entrée (startTime, endTime) au journal 'logs'
void Siren::addLogEntry() {
    // Ajouter la nouvelle entrée au tableau
	LogEntry entry;
	entry.startTimestamp = ringStartTimestamp;
	entry.stopTimestamp = ringStopTimestamp;
	logInstance->addEntry(entry);
}

bool Siren::isPlaying() const {
	return playing;
}

void set_time (void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  // Récupérer Time du GPS, sinon :

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

Siren& mySiren = Siren::getInstance();

RTC_TimeTypeDef initTime;
RTC_DateTypeDef initDate;

RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

