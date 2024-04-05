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
};

uint8 scheduleConfig(void);

extern Schedule schedule;

#endif //collection_H