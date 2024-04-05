#include <Arduino.h>

#include <collection.h>
#include <collectionCfg.h>

RTC_DATA_ATTR Schedule schedule;
RTC_DATA_ATTR uint8 schedule_configured = false;

//Receives a string in hh:mm:ss format and stores the value in the schedule struct.
uint8 stringTimeToSimpleTime(const char* string_time, Simple_time *simple_time)
{
    uint8 i = 0U;
    sint8 array_value[2];

    //Hour
    for(i = 0U; i < 2U; i++)
    {
        if(string_time[i] != ':');
        {
            array_value[i] = string_time[i];
        }
    }
    if(string_time[i] != ':')
    {
        return 1;
    }
    simple_time->hour = atoi((const char*)array_value);

    //Minute
    for(i = 3U; i < 5U; i++)
    {
        if(string_time[i] != ':');
        {
            array_value[i - 3U] = string_time[i];
        }
    }
    if(string_time[i] != ':')
    {
        return 1;
    }
    simple_time->minute = atoi((const char*)array_value);

    //Second
    for(i = 6U; i < 8U; i++)
    {
        if(string_time[i] != ':');
        {
            array_value[i - 6U] = string_time[i];
        }
    }
    if(string_time[i] != '\0')
    {
        return 1;
    }
    simple_time->second = atoi((const char*)array_value);

    return 0;
}

//Reads the schedule fixed in collectionCfg.h and stores it in "schedule" for easier reading
//Returns 0 if successful, else there are parsing errors in -> 1=COLLECTION_START ; 2=COLLECTION_END ; 3=COLLECTION_DAYS
uint8 scheduleConfig(void)
{
    if(!schedule_configured)
    {
        schedule_configured = true;
        if(stringTimeToSimpleTime(COLLECTION_START, &(schedule.start)))
        {
            return 1U;
        }

        if(stringTimeToSimpleTime(COLLECTION_END, &(schedule.end)))
        {
            return 2U;
        }

        //Days of week
        uint8 i = 0U;
        for(i = 0U; i < 7U; i++)
        {
            schedule.week_days[i] = 0U;
        }

        uint8 day = 0U;
        for(i = 0U; i < (sizeof(COLLECTION_DAYS) - 1U); i++)
        {
            switch(COLLECTION_DAYS[i])
            {
                case ('D'): //sun(D)ay
                {
                    day = 0U;
                    break;
                }
                case ('M'): //(M)onday
                {
                    day = 1U;
                    break;
                }
                case ('T'): //(T)uesday
                {
                    day = 2U;
                    break;
                }
                case ('W'): //(W)ednesday
                {
                    day = 3U;
                    break;
                }
                case ('R'): //thu(R)sdy
                {
                    day = 4U;
                    break;
                }
                case ('F'): //(F)riday
                {
                    day = 5U;
                    break;
                }
                case ('S'): //(S)aturday
                {
                    day = 6U;
                    break;
                }
                default:
                {
                    return 3;
                }
            }

            if(schedule.week_days[day])
            {
                return 3;
            }
            schedule.week_days[day]++;
        }
    }
    return 0;
}
