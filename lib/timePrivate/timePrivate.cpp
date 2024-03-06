#include <Arduino.h>

#include <timePrivate.h>

#include <esp_sleep.h>
#include <ESP32Time.h>

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

ESP32Time rtc(3600); //UTC + 1. THIS MIGHT BE RTC_DATA_ATTR?????

//Puts the MCU to sleep, and wakes it up again at the specified date and time.
//Returns 1 if specified date is not in the future.
uint8 sleepUntil(int year, int month, int day, int hour, int minute, int second)
{
  uint32 current_epoch = rtc.getLocalEpoch(); //save current epoch, without offset
  uint32 c_offset_epoch = rtc.getEpoch(); //save current epoch, with the offset of our timezone

  //get wakeup epoch
  rtc.setTime(second, minute, hour, day, month, year);
  uint32 wakeup_epoch = rtc.getLocalEpoch(); //save wakeup epoch, without offset in respect to the time set just above

  rtc.setTime(current_epoch); //restore current time

  if (wakeup_epoch > c_offset_epoch)
  {
    uint64 total_time_us = ((uint64)(wakeup_epoch - c_offset_epoch)) * 1000000;
    Serial.print("Sleeping for ");
    Serial.print(total_time_us / 1000000U);
    Serial.println(" s");
    esp_deep_sleep(total_time_us);
  }
  else
  {
    return 1;
  }
}
