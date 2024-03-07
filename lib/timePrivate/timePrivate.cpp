#include <Arduino.h>

#include <timePrivate.h>

#include <esp_sleep.h>

//Puts the MCU to sleep, and wates it up again after the specified amount of time has passed.
void sleepFor(uint16 seconds, uint16 minutes, uint16 hours, uint16 days)
{
  uint64 total_time_us = 0;

  total_time_us += 86400U * (uint64)days;
  total_time_us += 3600U * (uint64)hours;
  total_time_us += 60U * (uint64)minutes;
  total_time_us += (uint64)seconds;
    
  total_time_us *= 1000000U;
  Serial.print("Sleeping for ");
  Serial.print(total_time_us / 1000000U);
  Serial.println(" s");
  esp_deep_sleep(total_time_us);
}

//Puts the MCU to sleep, and wakes it up again at the specified date and time.
//Returns 1 if specified date is not in the future.
uint8 sleepUntil(int year, int month, int day, int hour, int minute, int second)
{
  struct tm wakeup_date;

  wakeup_date.tm_year = year - 1900;
  wakeup_date.tm_mon = month - 1;
  wakeup_date.tm_mday = day;
  wakeup_date.tm_hour = hour;
  wakeup_date.tm_min = minute;
  wakeup_date.tm_sec = second;
  wakeup_date.tm_isdst = -1;

  time_t wakeup_epoch = mktime(&wakeup_date);
  time_t current_epoch;
  time(&current_epoch);

  if (wakeup_epoch > current_epoch)
  {
    uint64 total_time_us = ((uint64)(wakeup_epoch - current_epoch)) * 1000000U;
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
