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
    uint8 old_data_id = out_packet[3];
    do
    {
      out_packet[3] = (uint8)random(0xFF);
    }while(out_packet[3] == old_data_id);
    
    out_packet[4] = (out_packet[4] + 1 ) % 32;
    Serial.println("Sleeping for 2 minutes");
    esp_deep_sleep(120*1000000); 
  }
}

void loop() {
  //unreachable
}
