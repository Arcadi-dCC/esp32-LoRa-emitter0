#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <LoRaCfg.h>
#include <customUtilities.h>
#include <esp_sleep.h>

RTC_DATA_ATTR uint8 out_packet [OUT_BUFFER_SIZE] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, EMITTER_ID, 0}; 

void setup() {
 
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  if(LoRaConfig())
  {
    SwReset(10);
  }
  delay(1000);

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

  if(sendPacket(out_packet, OUT_BUFFER_SIZE))
  {
    SwReset(10);
  }
  /*
  if(acknowledgement(5000))
  {
    Serial.println("Acknowledgement not received. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  else
  {
    out_packet[2] = (out_packet[2] + 1 ) % 32;
    Serial.print("Sleeping for 2 minutes");
    esp_deep_sleep(120*1000000); 
  }
  */
  out_packet[3] = (out_packet[3] + 1 ) % 32;
  Serial.print("Sleeping for 2 minutes");
  esp_deep_sleep(120*1000000); 
}

void loop() {
  //unreachable
}
