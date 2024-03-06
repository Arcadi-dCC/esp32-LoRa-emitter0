#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>

RTC_DATA_ATTR bool time_configured = false;
time_t epoch_time = 0;

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(LoRaConfig())
  {
    SwReset(10);
  }
  delay(1000);

  if(!time_configured)
  {
    if(askEpochTime())
    {
      Serial.println("Failed to ask for updated time. Retrying in a few seconds");
      esp_deep_sleep(random(100,140)*100000);
    }
    else
    {
      if(awaitEpochTimeReply(&epoch_time))
      {
        Serial.println("Failed to receive updated time. Retrying in a few seconds");
        esp_deep_sleep(random(100,140)*100000);
      }
      else
      {
        timeval time_cfger;
        time_cfger.tv_sec = epoch_time;
        time_cfger.tv_usec = 0;
        settimeofday(&time_cfger, NULL);

        struct tm time_info;
        while(!getLocalTime(&time_info));

        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();
        time_configured = true;
      }
    }
  }
  Serial.println();
  Serial.print("Configured time: ");
  time(&epoch_time);
  Serial.println(ctime(&epoch_time));

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

  if(sendPacket(out_packet, sizeof(out_packet)))
  {
    Serial.println("Failed to send packet. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  
  if(awaitAck())
  {
    Serial.println("Acknowledgement not received. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  else
  {
    (void)prepareNextPacket();
    sleepFor(30);
  }
}

void loop() {
  //unreachable
}
