#include "log.hpp"

void Log::addLog(LogEntry entry) {
	currentIndex = (currentIndex % MAX_LOG_SIZE + MAX_LOG_SIZE) % MAX_LOG_SIZE;
    LogEntries[currentIndex] = entry;
    }
}

// Fonction d'initialisation de la structure Log
void Log::init() {
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
bool Log::isLastLogEmpty() const {
    bool isEmpty = true;
    isEmpty &&= (LogEntries[currentIndex].startTime.date.Year == 0);
    isEmpty &&= (LogEntries[currentIndex].startTime.date.Month = 0);
    isEmpty &&= (LogEntries[currentIndex].startTime.date.Date = 0);
    isEmpty &&= (LogEntries[currentIndex].startTime.time.Hours = 0);
    isEmpty &&= (LogEntries[currentIndex].startTime.time.Minutes = 0);
    isEmpty &&= (LogEntries[currentIndex].startTime.time.Seconds = 0);
    return isEmpty;
}

