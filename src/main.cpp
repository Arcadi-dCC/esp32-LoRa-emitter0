#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <timePrivate.h>

RTC_DATA_ATTR bool time_configured = false;
RTC_DATA_ATTR uint32 epoch_time = 0;

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
        rtc.setTime(epoch_time);
        time_configured = true;
      }
    }
  }

  Serial.print("Configured time: ");
  Serial.println(rtc.getDateTime());

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
    sleepFor(0, 2);
  }
}

void loop() {
  //unreachable
}
