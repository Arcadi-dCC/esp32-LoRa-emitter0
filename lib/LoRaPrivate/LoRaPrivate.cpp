#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <LoRaPrivate.h>

bool Cad_isr_responded = false;
bool channel_busy = true;
bool ack_received = false;

//Encapsules the whole LoRa configuration. Returns 0 if successful, 1 if error.
uint8 LoRaConfig(uint8 sck, uint8 miso, uint8 mosi, uint8 ss, uint8 rst, uint8 dio0, uint32 freq, uint8 sf, uint32 bw)
{
  Serial.println("Configuring LoRa");
  //SPI LoRa pins
  SPI.begin(sck,miso,mosi,ss);
  
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    return 1;
  }
  
  LoRa.setSpreadingFactor(sf);        //LoRa Spreading Factor configuration.
  LoRa.setSignalBandwidth(bw);     //LoRa Bandwidth configuration.
  //LoRa.setPreambleLength(8);
  //LoRa.enableInvertIQ();
  //LoRa.setSyncWord(0x00);
  return 0;  
}

//Checks if someone is using the configured channel. Returns true if used, false if free
bool isChannelBusy(void)
{
  Cad_isr_responded = false;
  LoRa.channelActivityDetection();
  while(!Cad_isr_responded)
  {
    delay(1);
  }
  return channel_busy;
}

void onCadDone(bool signalDetected) //true = signal detected
{
  channel_busy = signalDetected;
  Cad_isr_responded = true;
}

//Waits for acknowledgement for some time. Timeout in ms. Returns 0 if ACK, 1 if no ACK
uint8 acknowledgement(uint16 timeout)
{
  uint16 start_time = millis();
  while((!ack_received) or (millis() < (start_time+timeout)))
  {
    delay(1);
  }
  if(ack_received)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//Called when the sending of data is finished. Not used
void onTxDone(void)
{
  
}

//Called when some data is received. Used for acknowledgement
void onReceive(int packetSize)
{
  //Checks if the packet has the exact length for GATEWAY_ID and EMITTER_ID to fit
  if(packetSize == 3)
  {
    uint8 in_packet[3];
    int w = 0;

    //Reads the ID values and compares to the established ID
    for(w = 0;w<3;w++) in_packet[w] = LoRa.read();
    if(in_packet[0] == (((GATEWAY_ID & 0xFF00) >> 8)) and (in_packet[1] == (GATEWAY_ID & 0x00FF)) and (in_packet[2] == EMITTER_ID))
    {
      ack_received = true;
    }
  } 
}
