#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>
#include <collection.h>

RTC_DATA_ATTR bool sent_OK = true;

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(LoRaConfig())
  {
    SwReset(10);
  }
  delay(1000);

  (void)checkTimeUpdate();

  if(timeConfigLoRa())
  {
    Serial.println("Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

  if(scheduleConfig())
  {
    Serial.println("Schedule has bad parsing. Check it in lib/collection/collectionCfg.h");
  }

  //TODO: go to sleep if scheduled collection end has been reached.
  struct tm now;
  if(!getLocalTime(&now))
  {
    (void)sleepFor(30);
  }

  bool after_end = false, before_start = false;

  if(now.tm_hour > schedule.end.hour)
  {
    after_end = true;
  }
  else if(now.tm_hour == schedule.end.hour)
  {
    if(now.tm_min > schedule.end.minute)
    {
      after_end = true;
    }
    else if(now.tm_min == schedule.end.minute)
    {
      if(now.tm_sec >= schedule.end.second)
      {
        after_end = true;
      }
    }
  }

  if(now.tm_hour < schedule.start.hour)
  {
    before_start = true;
  }
  else if(now.tm_hour == schedule.start.hour)
  {
    if(now.tm_min < schedule.start.minute)
    {
      before_start = true;
    }
    else if(now.tm_min == schedule.start.minute)
    {
      if(now.tm_sec < schedule.start.second)
      {
        before_start = true;
      }
    }
  }

  sint8 days_of_sleep = 0;

  if(schedule.active_at_midnight)
  {
    if(after_end and before_start)
    {
      while(!schedule.week_days[(now.tm_wday + days_of_sleep) % 7U])
      {
        days_of_sleep++;
      }
      (void)sleepUntil(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday + days_of_sleep, schedule.start.hour, schedule.start.minute, schedule.start.second);
    }
  }
  else
  {
    if(after_end)
    {
      days_of_sleep++;
      while(!schedule.week_days[(now.tm_wday + days_of_sleep) % 7U])
      {
        days_of_sleep++;
      }
      (void)sleepUntil(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday + days_of_sleep, schedule.start.hour, schedule.start.minute, schedule.start.second);
    }
    if(before_start)
    {
      while(!schedule.week_days[(now.tm_wday + days_of_sleep) % 7U])
      {
        days_of_sleep++;
      }
      (void)sleepUntil(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday + days_of_sleep, schedule.start.hour, schedule.start.minute, schedule.start.second);
    }
  }

  if(sent_OK)
  {
    (void)prepareNextPacket();
  }

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    sent_OK = false;
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

  if(sendPacket(out_packet, sizeof(out_packet)))
  {
    sent_OK = false;
    Serial.println("Failed to send packet. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  
  if(awaitAck())
  {
    sent_OK = false;
    Serial.println("Acknowledgement not received. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  else
  {
    sent_OK = true;

    if(!getLocalTime(&now))
    {
      (void)sleepFor(30);
    }

    //if garbage collection ends early in the morning, count one day less of sleep
    if(!schedule.active_at_midnight)
    {
      days_of_sleep++;
    }
    
    while(!schedule.week_days[(now.tm_wday + days_of_sleep) % 7U])
    {
      days_of_sleep++;
    }

    (void)sleepUntil(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday + days_of_sleep, schedule.start.hour, schedule.start.minute, schedule.start.second);
  }
}

void loop() {
  //unreachable
}
