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

    size_t getIndexPlusOffset(size_t i) const;
    void addEntry(LogEntry entry);
    void clear();
    bool isLastEntryEmpty() const;
    bool hasRungMoreThan(unsigned long duration) const;
    bool hasRungMoreThanXinX(uint32_t duration, uint32_t durationRef) const;

};

uint32_t elapsedTime(LogTime LogTime1, LogTime LogTime2);
LogTime getTime(void);
LogTime AddSecondToTime(uint32_t offsetSeconds, LogTime oriLogTime);

#endif // LOG_H
