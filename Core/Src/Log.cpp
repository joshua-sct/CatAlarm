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
        LogEntries[i].startTimestamp = 0;
        LogEntries[i].stopTimestamp = 0;
    }
}

// Test si la dernière entrée du journal est vide
bool Log::isLastEntryEmpty() const {
    bool isEmpty = (LogEntries[currentIndex].startTimestamp == 0)
    && (LogEntries[currentIndex].stopTimestamp == 0);
    return isEmpty;
}

// Détermine si la sonnerie à sonné plus longtemps que "duration"
bool Log::hasRungMoreThan(uint32_t duration) const {
    uint32_t totalDuration = 0;
    uint32_t deltaBetween = 0;
	int i = currentIndex; 
    int j;
    uint32_t currentTimestamp = getTimestamp();
    // Tant que le log considéré correspond à la sonnerie actuelle
    deltaBetween = currentTimestamp - LogEntries[i].stopTimestamp;
    while (deltaBetween < SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS) {
        // Ajout de la durée du log actuel
        totalDuration += LogEntries[i].stopTimestamp - LogEntries[i].startTimestamp;
        // Calcul de la distance au log précédent
        j = getIndexPlusOffset(-1);
        deltaBetween = LogEntries[j].stopTimestamp - LogEntries[i].startTimestamp;
        i = j;
    }
    return (totalDuration > duration);
}

bool Log::hasRungMoreThanXinX(uint32_t duration, uint32_t durationRef) const {
    uint32_t totalDuration = 0;
    uint32_t deltaBetween = 0;

    uint32_t currentTimestamp = getTimestamp();
    uint32_t clippedTimestamp;
    
    int i = currentIndex;
    int j = 0;
    
    // Calcul de la distance au dernier log
    deltaBetween = LogEntries[i].stopTimestamp - currentTimestamp;
    
    // Vérifie si un log est assez récent pour être considéré ET que tous les logs n'ont pas été parcourus
    while (deltaBetween < durationRef && j < MAX_LOG_SIZE) {
        // Calcule la durée du log actuel
        uint32_t logDuration = LogEntries[i].stopTimestamp - LogEntries[i].startTimestamp;
        // Vérifie si l'entiereté du log est plus récent que durationRef
        if ((deltaBetween + logDuration) < durationRef) {
            totalDuration += logDuration;
        }
        // Sinon, une partie seulement du log est plus récente que durationRef
        else {
            // La partie concernée s'étend de stopTime à (currentTime-durationRef)
            clippedTimestamp = currentTimestamp - durationRef;
            totalDuration += LogEntries[i].stopTimestamp - clippedTimestamp;
        }        
        // Calcul de la distance au log précédent
        i = getIndexPlusOffset(-1);
        deltaBetween = LogEntries[i].stopTimestamp - currentTimestamp;
        j++;
    }
    return (totalDuration > duration);
}

// Renvoie le Timestamp actuel
uint32_t getTimestamp(void)
{
    DateTime currentTime;
    HAL_RTC_GetTime(&hrtc, &currentTime.time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentTime.date, RTC_FORMAT_BIN);
    return DateTimeToTimestamp(currentTime);
}

// Convertie un Datetime en Timestamp UNIX
uint32_t DateTimeToTimestamp(DateTime DateTime)
{
	int i;
	uint32_t sec;

	sec = DateTime.date.Year * SECS_PER_YEAR;
	for (i = 0; i < DateTime.date.Year; i++) {
		if (IS_LEAP_YEAR(i))
			sec += SECS_PER_DAY;
	}

	sec += (days_since_year_start[DateTime.date.Month - 1] +
		(IS_LEAP_YEAR(DateTime.date.Year) && DateTime.date.Month > 2) +
		(DateTime.date.Date - 1)) * SECS_PER_DAY;

    sec += DateTime.time.Hours * 3600 + DateTime.time.Minutes * 60 + DateTime.time.Seconds;
	// Ajout des secondes écoulées entre 1970 et 2000
	return sec + SECS_TILL_YEAR_2K;
}
