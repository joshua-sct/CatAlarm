#ifndef LOG_H
#define LOG_H

#include <array>
#include "rtc.h"

// Taille maximale du tableau de logs
#define MAX_LOG_SIZE 100

struct LogTime {
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
};

// Structure pour un log
struct LogEntry {
	LogTime startTime;
	LogTime stopTime;
};

struct Log {
    std::array<LogEntry, MAX_LOG_SIZE> LogEntries; // Tableau pour stocker les logs
    size_t currentIndex = 0;

    void addLog(LogEntry entry);
    void init();
};

#endif // LOG_H
