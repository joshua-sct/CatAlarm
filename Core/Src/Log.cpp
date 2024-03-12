#include "log.hpp"
#include "global.h"
#include <cmath>

size_t Log::getIndexPlusOffset(size_t i) const {
    return (currentIndex + i % MAX_LOG_SIZE + MAX_LOG_SIZE) % MAX_LOG_SIZE;
}

void Log::addEntry(LogEntry entry) {
    currentIndex = getIndexPlusOffset(1);
    LogEntries[currentIndex] = entry;
}

// Fonction d'initialisation de la structure Log
void Log::clear() {
    // Réinitialiser l'index courant à zéro
    currentIndex = 0;

    // Initialiser chaque élément du tableau LogEntries
    for (size_t i = 0; i < MAX_LOG_SIZE; ++i) {
        // Pour chaque élément, initialisez les champs de startTime et stopTime à des valeurs par défaut
        LogEntries[i].startTime.date.Year = 0;
        LogEntries[i].startTime.date.Month = 0;
        LogEntries[i].startTime.date.Date = 0;
        LogEntries[i].startTime.time.Hours = 0;
        LogEntries[i].startTime.time.Minutes = 0;
        LogEntries[i].startTime.time.Seconds = 0;

        LogEntries[i].stopTime.date.Year = 0;
        LogEntries[i].stopTime.date.Month = 0;
        LogEntries[i].stopTime.date.Date = 0;
        LogEntries[i].stopTime.time.Hours = 0;
        LogEntries[i].stopTime.time.Minutes = 0;
        LogEntries[i].stopTime.time.Seconds = 0;
    }
}

// Test si la dernière entrée du journal est vide
bool Log::isLastEntryEmpty() const {
    bool isEmpty = (LogEntries[currentIndex].startTime.date.Year == 0)
    && (LogEntries[currentIndex].startTime.date.Month == 0)
    && (LogEntries[currentIndex].startTime.date.Date == 0)
    && (LogEntries[currentIndex].startTime.time.Hours == 0)
    && (LogEntries[currentIndex].startTime.time.Minutes == 0)
    && (LogEntries[currentIndex].startTime.time.Seconds == 0);
    return isEmpty;
}

// Détermine si la sonnerie à sonner plus longtemps que "duration"
bool Log::hasRungMoreThan(unsigned long duration) const {
    uint32_t totalDuration = 0;
    uint32_t deltaBetween = 0;
	int i = currentIndex; 
    int j;
    LogTime currentTime = getTime(); 
    // Vérifie si le log est pas trop vieux
    deltaBetween = elapsedTime(LogEntries[i].stopTime, currentTime);
    while (deltaBetween < SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS) {
        // Ajout de la durée du log actuel
        totalDuration += elapsedTime(LogEntries[i].startTime, LogEntries[i].stopTime);
        // Calcul de la distance au log précédent
        j = getIndexPlusOffset(-1);
        deltaBetween = elapsedTime(LogEntries[j].stopTime, LogEntries[i].startTime);
        i = j;
    }
    return (totalDuration > duration);
}

bool Log::hasRungMoreThanXinX(uint32_t duration, uint32_t durationRef) const {
    uint32_t totalDuration = 0;
    uint32_t deltaBetween = 0;

    LogTime currentTime = getTime();
    LogTime clippedTime;
    
    int i = currentIndex;
    int j = 0;
    
    // Calcul de la distance au dernier log
    deltaBetween = elapsedTime(LogEntries[i].stopTime, currentTime);
    
    // Vérifie si un log est assez récent pour être considéré ET que tous les logs n'ont pas été parcourus
    while (deltaBetween < durationRef && j < MAX_LOG_SIZE) {
        // Calcule la durée du log actuel
        uint32_t logDuration = elapsedTime(LogEntries[i].startTime, LogEntries[i].stopTime);
        // Vérifie si l'entiereté du log est plus récent que durationRef
        if ((deltaBetween + logDuration) < durationRef) {
            totalDuration += logDuration;
        }
        // Sinon, une partie seulement du log est plus récente que durationRef
        else {
            // La partie concernée s'étend de stopTime à (currentTime-durationRef)
            clippedTime = AddSecondToTime(-durationRef, currentTime);
            totalDuration += elapsedTime(clippedTime, LogEntries[i].stopTime);
        }        
        // Calcul de la distance au log précédent
        i = getIndexPlusOffset(-1);
        deltaBetween = elapsedTime(LogEntries[i].stopTime, currentTime);
        j++;
    }
    return (totalDuration > duration);
}

// Renvoie le nombre de seconde sentre 2 LogTime
uint32_t elapsedTime(LogTime LogTime1, LogTime LogTime2)
{
	// Log1 < Log2
	int8_t elapsedYear =




	// Convertir les heures en secondes et les jours en secondes pour LogTime1
	uint32_t Seconds1 = LogTime1.time.Hours * 3600 + LogTime1.time.Minutes * 60 + LogTime1.time.Seconds;
    Seconds1 += (LogTime1.date.Year * 365 + LogTime1.date.Month * 30 + LogTime1.date.Date - 1) * 86400;
	// Convertir les heures en secondes et les jours en secondes pour LogTime2
	uint32_t Seconds2 = LogTime2.time.Hours * 3600 + LogTime2.time.Minutes * 60 + LogTime2.time.Seconds;
    Seconds2 += (LogTime2.date.Year * 365 + LogTime2.date.Month * 30 + LogTime2.date.Date - 1) * 86400;
	// Retourne la différence de temps en secondes
	return abs(Seconds2 - Seconds1);
}

// Renvoie le temps LogTime actuel
LogTime getTime(void)
{
    LogTime currentTime;
    HAL_RTC_GetTime(&hrtc, &currentTime.time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentTime.date, RTC_FORMAT_BIN);
    return currentTime;
}

LogTime AddSecondToTime(uint32_t offsetSeconds, LogTime oriLogTime) {
    // Convertir le temps en secondes
    uint32_t oriSeconds = oriLogTime.time.Hours * 3600 + oriLogTime.time.Minutes * 60 + oriLogTime.time.Seconds;
    oriSeconds += (oriLogTime.date.Year * 365 + oriLogTime.date.Month * 30 + oriLogTime.date.Date - 1) * 86400;

    
    // Ajouter l'offset en secondes
    uint32_t totalSeconds = oriSeconds + offsetSeconds;

    // Calculer la nouvelle date
    uint8_t newDays = totalSeconds / 86400 + 1;

    // Calculer le nouvel heure, minute et seconde
    totalSeconds %= 86400;
    uint8_t newHours = totalSeconds / 3600;
    uint8_t newMinutes = (totalSeconds % 3600) / 60;
    uint8_t newSeconds = totalSeconds % 60;
    
    // Créer un nouveau LogTime avec les nouvelles valeurs
    LogTime newLogTime;
    newLogTime.date = newDays;
    newLogTime.time.Hours = newHours;
    newLogTime.time.Minutes = newMinutes;
    newLogTime.time.Seconds = newSeconds;
    return newLogTime;
}
