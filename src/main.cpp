#include <Arduino.h>
#include <platformTypes.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>
#include <esp_sleep.h>

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

  if(sendPacket(out_packet, sizeof(out_packet)))
  {
    SwReset(10);
  }
  
  if(awaitAck())
  {
    Serial.println("Acknowledgement not received. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }
  else
  {
    out_packet[3] = (out_packet[3] + 1 ) % 32;
    Serial.println("Sleeping for 2 minutes");
    esp_deep_sleep(30*1000000); 
  }
}

void loop() {
  //unreachable
}
