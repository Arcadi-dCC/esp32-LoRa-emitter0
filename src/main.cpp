#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>
#include <collection.h>
#include <AJ-SR04M_Drv.h>

#define MIN_RETRY_DELAY 5000U //ms
#define MAX_RETRY_DELAY 10000U //ms

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(scheduleConfig())
  {
    Serial.println("Schedule has something wrong. Check it in lib/collection/collectionCfg.cpp and .h");
    (void)sleepFor(0, 0, 0, 0xFFFF);
  }

  if(AJ_SR04M_Config())
  {
    Serial.println("Selected mode of operation for distance sensor is not implemented.");
    Serial.println("Check OP_MODE in lib/AJ-SR04M_Drv/AJ-SR04M_Cfg.h");
    (void)sleepFor(0, 0, 0, 0xFFFF);
  }

  if(LoRaConfig())
  {
    SwReset(10);
  }
  delay(1000);

  (void)checkTimeUpdate();

  if(timeConfigLoRa())
  {
    Serial.println("Retrying in a few seconds");
    esp_deep_sleep(random(MIN_RETRY_DELAY,MAX_RETRY_DELAY)*1000U);
  }

  switch(outOfScheduleManager())
  {
    case 1U:
    {
      Serial.println("Something is wrong with the schedule. Check it in lib/collection/collectionCfg.h");
      (void)sleepFor(0, 0, 0, 0xFFFF);
      break;
    }
    case 2:
    {
      sleepFor(30);
      break;
    }
    default:
    {
      /*Do nothing*/
    }
  }

  if(prepareNextPacket())
  {
    Serial.println("Sending old distance value since sensor did not respond.");
  }

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(MIN_RETRY_DELAY,MAX_RETRY_DELAY)*1000U);
  }

  //Retry in a few seconds if failed to send packet
  if(sendPacket(out_packet, sizeof(out_packet)))
  {
    Serial.println("Failed to send packet. Retrying in a few seconds");
    esp_deep_sleep(random(MIN_RETRY_DELAY,MAX_RETRY_DELAY)*1000U);
  }
  
  //Retry in a few seconds if acknowledgement was not received
  if(awaitAck())
  {
    Serial.println("Acknowledgement not received. Retrying in a few seconds");
    esp_deep_sleep(random(MIN_RETRY_DELAY,MAX_RETRY_DELAY)*1000U);
  }
  else
  {
    
    switch(finishedTodaysJobManager())
    {
      case 1U:
      {
        Serial.println("Something is wrong with the schedule. Check it in lib/collection/collectionCfg.h");
        (void)sleepFor(0, 0, 0, 0xFFFF);
        break;
      }
      case 2:
      {
        sleepFor(30);
        break;
      }
      default:
      {
        /*Do nothing*/
      }
    }
  }
}

void loop() {
  //unreachable
}
