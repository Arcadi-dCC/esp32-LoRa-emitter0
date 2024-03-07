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

  switch(timeConfigLoRa())
  {
    case 1U:
    {
      Serial.println("Failed to ask for updated time. Retrying in a few seconds");
      esp_deep_sleep(random(100,140)*100000);
      break;
    }
    case 2U:
    {
      Serial.println("Failed to receive updated time. Retrying in a few seconds");
      esp_deep_sleep(random(100,140)*100000);
      break;
    }
    case 3U:
    {
      Serial.println("Failed to update time internally.");
      SwReset(10);
      break;
    }
    default:
    {
      //Do nothing
    }
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
    sleepFor(30);
  }
}

void loop() {
  //unreachable
}
