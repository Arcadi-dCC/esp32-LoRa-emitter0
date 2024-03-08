#include <Arduino.h>

#include <timePrivate.h>
#include <timeCfg.h>

#include <LoRaPrivate.h>
#include <esp_sleep.h>

RTC_DATA_ATTR bool time_configured = false;

//On the first run, asks the gateway for the current calendar time via LoRa, and then updates the emitter clock internally.
//After the first run, updates timezone info and shows the time.
//Time can be reconigured after a wakeup by setting time_configured variable to false before going to sleep.
//Returns 0 if successful, 1 if emitter could not ask for calendar time, 2 if gateway did not respond to the petition, 3 if time config API failed internally.
uint8 timeConfigLoRa(void)
{
  time_t cldtime = 0;

  if(!time_configured)
  {
    if(askCalendarTime())
    {
      Serial.println("Failed to ask for updated time.");
      return 1U;
    }

    if(awaitCalendarTimeReply(&cldtime))
    {
      Serial.println("Failed to receive updated time.");
      return 2U;
    }

    timeval time_cfger;
    time_cfger.tv_sec = cldtime;
    time_cfger.tv_usec = 0;
    settimeofday(&time_cfger, NULL);
    struct tm time_info;

    if(!getLocalTime(&time_info, TCUPD_TIMEOUT))
    {
      Serial.println("Failed to update time internally.");
      return 3U;
    }
    
    time_configured = true;
  }

  setenv("TZ", TZ_INFO, 1);
  tzset();
  time(&cldtime);
  Serial.print("Time is: ");
  Serial.print(ctime(&cldtime));

  return 0U;
}

//Puts the MCU to sleep, and wakes it up again after the specified amount of time has passed.
void sleepFor(uint16 seconds, uint16 minutes, uint16 hours, uint16 days)
{
  uint64 total_time_us = 0;

  total_time_us += 86400U * (uint64)days;
  total_time_us += 3600U * (uint64)hours;
  total_time_us += 60U * (uint64)minutes;
  total_time_us += (uint64)seconds;
    
  total_time_us *= 1000000U; //conversion to us
  Serial.print("Sleeping for ");
  Serial.print(total_time_us / 1000000U);
  Serial.println(" s");
  esp_deep_sleep(total_time_us);
}

//Puts the MCU to sleep, and wakes it up again at the specified date and time.
//Returns 1 if specified date is not in the future.
uint8 sleepUntil(int year, int month, int day, int hour, int minute, int second)
{
  //insert date in tm struct with expected format
  struct tm wakeup_date;

  wakeup_date.tm_year = year - 1900;
  wakeup_date.tm_mon = month - 1;
  wakeup_date.tm_mday = day;
  wakeup_date.tm_hour = hour;
  wakeup_date.tm_min = minute;
  wakeup_date.tm_sec = second;
  wakeup_date.tm_isdst = -1;

  time_t wakeup_cldtime = mktime(&wakeup_date); //get calendar time of wake up date
  time_t current_cldtime;
  time(&current_cldtime); //get current calendar time

  if (wakeup_cldtime > current_cldtime)
  {
    //calculate number of us left for reaching wake up date
    uint64 total_time_us = ((uint64)(wakeup_cldtime - current_cldtime)) * 1000000U;
    Serial.print("Waking up on ");
    Serial.print(asctime(&wakeup_date));

    Serial.print("Sleeping for ");
    Serial.print(total_time_us / 1000000U);
    Serial.println(" s");
    esp_deep_sleep(total_time_us);
  }
  else
  {
    Serial.println("Wake up date is not in the future");
    return 1;
  }
}
