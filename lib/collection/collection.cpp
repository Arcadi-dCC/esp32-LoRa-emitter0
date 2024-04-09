#include <Arduino.h>

#include <collection.h>
#include <collectionCfg.h>

#include <timePrivate.h>

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
        
        if(sizeof(COLLECTION_DAYS) <= 1U)
        {
            return 3;
        }

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

    //Check if collection is happenning at midnight
    schedule.active_at_midnight = isInstantBeforeReference(schedule.end, schedule.start);

    return 0;
}

//Compares a given instance with a reference, and returns if the instance is before in time or not.
//If instant is the exact same as reference, it returns false.
bool isInstantBeforeReference(Simple_time instant, Simple_time reference)
{
    if(instant.hour < reference.hour)
  {
    return true;
  }
  else if(instant.hour == reference.hour)
  {
    if(instant.minute < reference.minute)
    {
      return true;
    }
    else if(instant.minute == reference.minute)
    {
      if(instant.second < reference.second)
      {
        return true;
      }
    }
  }
  return false;
}

//Puts the MCU into sleep mode until the next garbage collection start.
//Asks if you want an additional day of sleep. This comes in handy for the rest of the code.
//Returns 1 if  wakeup date is not in the future.
uint8 sleepManager(tm tm_now, int extra_day_sleep = 0)
{   
    int days_of_sleep = 0;
    days_of_sleep += extra_day_sleep;

    while(!schedule.week_days[(tm_now.tm_wday + days_of_sleep) % 7U])
    {
      days_of_sleep++;
    }

    return sleepUntil(tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday + days_of_sleep, schedule.start.hour, schedule.start.minute, schedule.start.second);
}

//Checks if the code is running out of schedule through some logic. If so, it calls sleepManager.
//Returns 0 if in schedule, 1 if wakeup date is not in the future, 2 if current time could not be retrieved.
uint8 outOfScheduleManager(void)
{
    Simple_time now;
    struct tm tm_now;
    if(!getLocalTime(&tm_now))
    {
      return 2U;
    }

    now.hour = tm_now.tm_hour;
    now.minute = tm_now.tm_min;
    now.second = tm_now.tm_sec;

    bool after_end = !(isInstantBeforeReference(now, schedule.end));
    bool before_start = isInstantBeforeReference(now, schedule.start);

    //Is there collection today?
    int day_to_check = tm_now.tm_wday;

    if(schedule.active_at_midnight and before_start)
    {
        day_to_check--;
        if(day_to_check < 0)
        {
            day_to_check = 6;
        }
    }
    if(!schedule.week_days[day_to_check])
    {
        return sleepManager(tm_now);
    }

    //Are we in the collection schedule or out of it?
    if(schedule.active_at_midnight)
    {
        if(after_end and before_start)
        {
            return sleepManager(tm_now);
        }
    }
    else
    {
        if(after_end)
        {
            return sleepManager(tm_now, 1);
        }

        if(before_start)
        {
            return sleepManager(tm_now);
        }
    }
    return 0U;
}

//Function called when the information has reached the gateway. It puts the MCU to sleep until next garbage collection.
//Returns 1 if wakeup date is not in the future, 2 if current time could not be retrieved.
uint8 finishedTodaysJobManager(void)
{
    int extra_day_sleep = 1;

    struct tm tm_now;
    if(!getLocalTime(&tm_now))
    {
      return 2U;
    }
    Simple_time now;
    now.hour = tm_now.tm_hour;
    now.minute = tm_now.tm_min;
    now.second = tm_now.tm_sec;

    if(schedule.active_at_midnight and isInstantBeforeReference(now, schedule.start))
    {
        extra_day_sleep--;
    }
    
    return sleepManager(tm_now, extra_day_sleep);

}
