#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>
#include <collection.h>

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(scheduleConfig())
  {
    Serial.println("Schedule has bad parsing. Check it in lib/collection/collectionCfg.h");
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
    esp_deep_sleep(random(100,140)*100000);
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

 (void)prepareNextPacket();

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
