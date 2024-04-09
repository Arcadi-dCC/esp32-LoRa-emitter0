#ifndef collection_H
#define collection_H

#include <platformTypes.h>

struct Simple_time
{
    uint8 hour, minute, second;
};

struct Schedule
{
    Simple_time start;
    Simple_time end;
    uint8 week_days[7];
    bool active_at_midnight;
};

uint8 scheduleConfig(void);
bool isInstantBeforeReference(Simple_time instant, Simple_time reference);
uint8 sleepManager(tm tm_now, int extra_day_sleep);
uint8 outOfScheduleManager(void);
uint8 finishedTodaysJobManager(void);

extern Schedule schedule;

#endif //collection_H