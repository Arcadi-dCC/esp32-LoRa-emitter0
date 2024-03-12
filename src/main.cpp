#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(LoRaConfig())
  {
    SwReset(10);
  }
  delay(1000);

  if(timeConfigLoRa())
  {
    Serial.println("Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

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
    (void)checkTimeUpdate();

    struct tm time_info;

    if(!getLocalTime(&time_info))
    {
      (void)sleepFor(0, 5);
    }
    if(time_info.tm_hour == 23)
    {
      (void)sleepUntil(time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday + 1, 8, 0, 0);
    }
    else
    {
      (void)sleepFor(30);
    }


    
    
  }
}

void loop() {
  //unreachable
}
