#include <platformTypes.h>

#include <Arduino.h>
#include <esp_sleep.h>
#include <SPI.h>
#include <LoRa.h>

#include <LoRaPrivate.h>
#include <customUtilities.h>

#define SCK 5     //Clock Signal
#define MISO 19   //Master input Slave output
#define MOSI 27   //Master output Slave input
#define SS 18     //Slave Selection
#define RST 14    //Reset PIN
#define DIO0 26   //Digital Input-Output

RTC_DATA_ATTR uint8 out_packet [4] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, EMITTER_ID, 0}; 


void setup() {
 
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  if(LoRaConfig(SCK, MISO, MOSI, SS, RST, DIO0, 868E6, 7, 250E3))
  {
    SwReset(10);
  }
  LoRa.onCadDone(onCadDone);
  LoRa.onTxDone(onTxDone);
  LoRa.onReceive(onReceive);
  
  delay(1000);

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(100,140)*100000);
  }

  // send packet
  Serial.print("Sending: ");
  printStr(out_packet, sizeof(out_packet));
  Serial.println();
  while(!LoRa.beginPacket());
  //LoRa.print("0");                //Configurat per enviar 0 sempre (per no anar variant el tamany dels paquets)
  LoRa.write(out_packet, sizeof(out_packet));
  LoRa.endPacket(false); //blocking mode
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
  out_packet[2] = (out_packet[2] + 1 ) % 32;
  Serial.print("Sleeping for 2 minutes");
  esp_deep_sleep(120*1000000); 
}

void loop() {
  //unreachable
}
