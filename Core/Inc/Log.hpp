#ifndef LOG_H
#define LOG_H

#include "rtc.h"			// HAL RTC
#include <array>			// Tableaux de taille fixe

#define SECS_PER_MINUTE     60
#define SECS_PER_HOUR       (60 * SECS_PER_MINUTE)
#define SECS_PER_DAY        (24 * SECS_PER_HOUR)
#define SECS_PER_WEEK       (7 * SECS_PER_DAY)
#define SECS_PER_YEAR       (365 * SECS_PER_DAY)
// The seconds elapsed from 01-01-1970 to 01-01-2000
#define SECS_TILL_YEAR_2K   (946684800)
#define IS_LEAP_YEAR(x)     (((x) % 4 == 0) && (((x) % 100 != 0) || ((x) % 400 == 0)))

// Taille maximale du tableau de logs
#define MAX_LOG_SIZE 100

inline uint16_t days_since_year_start[12] = {
0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

struct DateTime {
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
};

// Structure pour un log
struct LogEntry {
	uint32_t startTimestamp;
	uint32_t stopTimestamp;
};

class Log {
public:
    static Log& getInstance(); // Méthode pour obtenir l'instance unique

    // Autres méthodes de la structure Log
    size_t getIndexPlusOffset(size_t i) const;
    void addEntry(LogEntry entry);
    void init();
    bool isLastEntryEmpty() const;
    bool hasRungMoreThan(uint32_t duration) const;
    bool hasRungMoreThanXinX(uint32_t duration, uint32_t durationRef) const;
    void setTime(void);

private:
    Log() = default;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    std::array<LogEntry, MAX_LOG_SIZE> LogEntries;
    size_t currentIndex;
};

uint32_t getTimestamp(void);
uint32_t DateTimeToTimestamp(DateTime DateTime);

#endif // LOG_H
