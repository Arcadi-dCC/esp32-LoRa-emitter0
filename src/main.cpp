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

    struct tm time_info;

    if(!getLocalTime(&time_info))
    {
      (void)sleepFor(0, 5);
    }
    if(time_info.tm_hour == 8)
    {
      (void)sleepUntil(time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday, 17, 0, 0); //do not send from 8 AM to 5 PM.
    }
    else
    {
      (void)sleepFor(0, 5);
    }
  }
}

void loop() {
  //unreachable
}
