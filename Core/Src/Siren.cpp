// Inclusions des bibliothèques nécessaires
#include <Siren.hpp>		// Gestion de Siren
#include "global.h"			// Paramètres globaux
#include "error.hpp"		// Gestion d'erreurs
#include "stm32g0xx_hal.h"
#include "alt_main.h"
#include "sigfox.hpp"
#include "gpio.h"

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


// Constructeur de Siren
Siren(Log* ptrLog) : log(ptrLog) {}

// Gère la sirène lors de son appel
void Siren::handleStart() {
	// Première sonnerie
	if (isLastLogEmpty()) {
		// Première sonnerie
		playIntermittentTone(durationMinimal);
	}
	
	// n-ième sonnerie
	else {
		// Si la sirène a trop sonné en continu
		if (hasSoundedMoreThan(durationMaximal)) {
			setError(errorSirenHasBeenPlayingForTooLong, true);
		}

		// Si la sirène a trop sonné pendant une période donnée
		else if (hasSoundedMoreThanXinX(durationMaxInDurationRef, durationRef)) {
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
void Siren::playIntermittentTone(unsigned long duration) {
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
	recordSirenTrigger(intermittentToneStartTime, intermittentToneEndTime);
}

void Siren::playTone(unsigned long duration) {
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
	recordSirenTrigger(intermittentToneStartTime, intermittentToneEndTime);
}

// Joue un bip de sirène
void Siren::playQuickTone() {
	// Return if ok
	handleStart();
	HAL_Delay(intervalDuration / 2); // Temps de tonalité
	handleStop();
}

// Ajoute une entrée (startTime, endTime) au journal 'logs'
void Siren::recordSirenTrigger(Timing startTiming, Timing stopTiming) {
    // Ajouter la nouvelle entrée au tableau
    logs[logIndex].startTiming = startTiming;
    logs[logIndex].stopTiming = stopTiming;

    // Incrémenter l'index du tableau
    logIndex = (logIndex + 1) % logSize;
}

// Détermine si la sonnerie à sonner plus longtemps que "duration"
bool Siren::hasSoundedMoreThan(unsigned long duration) const {
    unsigned long totalDuration = 0;
	int i = logIndex; 

	//* Tant que le log actuel correspond à la même sonnerie que celle qui va sonner
//	while (millis() - logs[i].endTiming < minDelayBetweenTwoTriggers) {
//		// Ajouter la durée de la sonnerie du log actuel
//		totalDuration += (logs[i].endTiming - logs[i].startTiming);
//		// Passer au log précédent
//		i = positiveModulo(i - 1, logSize);
//		}

	return (totalDuration >= duration);
}

// Détermine si la sonnerie à sonner plus longtemps que "duration" sur la période "durationRef"
bool Siren::hasSoundedMoreThanXinX(unsigned long duration, unsigned long durationRef) const {
    unsigned long totalDuration = 0;
	int i = logIndex;
	int j = 0;

	//* Tant que le log actuel n'as pas sonné plus tard qu'il y a durationRef et que la liste entière n'as pas été entièrement parcourue
//	while ((logs[i].startTiming > millis() - durationRef) && j < logSize) {
//		// Ajouter la durée de la sonnerie du log actuel
//		totalDuration += (logs[i].endTiming - logs[i].startTiming);
//		// Passer au log précédent
//		i = positiveModulo(i - 1, logSize);
//		// Incrémenter le nombre d'élément parcouru dans la liste
//		j++;
//	}

	return (totalDuration >= duration);
}

bool Siren::isPlaying() const {
	return playing;
}

// Destructeur de Siren
Siren::~Siren() {}

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


RTC_TimeTypeDef initTime;
RTC_DateTypeDef initDate;

RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

void get_time(void)
{
 HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
 HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
}

int32_t differenceSeconds;

void dif_time(RTC_DateTypeDef g1Date, RTC_TimeTypeDef g1Time, RTC_DateTypeDef g2Date, RTC_TimeTypeDef g2Time)
{
	// Convertir la différence des 2 moments en secondes
	uint32_t g1Seconds = g1Time.Hours * 3600 + g1Time.Minutes * 60 + g1Time.Seconds;
	g1Seconds += (g1Date.Date - 1) * 86400; // Ajouter les secondes pour les jours écoulés
	uint32_t g2Seconds = g2Time.Hours * 3600 + g2Time.Minutes * 60 + g2Time.Seconds;
	g2Seconds += (g2Date.Date - 1) * 86400; // Ajouter les secondes pour les jours écoulés
	differenceSeconds = g2Seconds - g1Seconds;

	//	// Calculer le temps écoulé en jours, heures, minutes et secondes
	//	int32_t jours = differenceSeconds / 86400;
	//	int32_t heures = (differenceSeconds % 86400) / 3600;
	//	int32_t minutes = (differenceSeconds % 3600) / 60;
	//	int32_t secondes = differenceSeconds % 60;
}
