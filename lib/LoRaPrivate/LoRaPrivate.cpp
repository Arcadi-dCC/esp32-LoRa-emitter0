#include <Arduino.h>

#include <LoRaPrivate.h>

#include <SPI.h>
#include <LoRa.h>
#include <customUtilities.h>

RTC_DATA_ATTR uint8 out_packet [OUT_BUFFER_SIZE] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, EMITTER_ID, 0};

bool Cad_isr_responded = false;
bool channel_busy = true;
bool ack_received = false;

//Encapsules the whole LoRa configuration. Returns 0 if successful, 1 if error.
uint8 LoRaConfig(void)
{
  Serial.println("Configuring LoRa");
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(FREQ)) {
    Serial.println("Starting LoRa failed!");
    return 1;
  }
  
  LoRa.setSpreadingFactor(SPR_FACT);
  LoRa.setSignalBandwidth(BANDWIDTH);
  //LoRa.setPreambleLength(PREAM_LEN);
  //LoRa.enableInvertIQ();
  //LoRa.setSyncWord(SYNC_WORD);

  LoRa.onCadDone(onCadDone); //call onCadDone ISR when channel activity detection has finished
  //LoRa.onTxDone(onTxDone); //call onTxDone ISR when packet has been fully sent
  LoRa.onReceive(onReceive); //call onReceive ISR when signals are received
  
  return 0;  
}

//Blocking. Checks if someone is using the configured channel. Returns true if used, false if free
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

void onCadDone(bool signalDetected) //true menas signal is detected
{
  channel_busy = signalDetected;
  Cad_isr_responded = true;
}

//Sends a packet through LoRa. Returns 0 if successful, 1 if error
uint8 sendPacket(uint8* packet, uint16 packet_len)
{
  Serial.print("Sending: ");
  printStr(packet, packet_len);
  Serial.println();
  while(!LoRa.beginPacket()); // a timeout could go here
  LoRa.write(packet, packet_len);
  LoRa.endPacket(false); //blocking mode

  return 0;
}

//Waits for acknowledgement for some time. Timeout set in Cfg file. Returns 0 if ACK, 1 if no ACK
uint8 awaitAck(void)
{
  LoRa.receive();
  uint16 start_time = millis();
  while((!ack_received) and (millis() < (start_time + ACK_TIMEOUT))) //might fail once every 50 days
  {
    delay(1);
  }
  if(ack_received)
  {
    ack_received = false;
    return 0;
  }
  else
  {
    return 1;
  }
}

/* NOT USABLE IN EMITTER
//Sends a small packet with GATEWAY_ID and the EMITTER_ID received through LoRa. Returns 0 if OK, 1 if error
uint8 replyAck(void)
{
  uint8 out_packet[3] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, in_packet[GATEWAY_ID_LEN]};
  uint8 returner = sendPacket(out_packet, sizeof(out_packet));
  LoRa.receive();
  return returner;
}
*/

//Called when the sending of data is finished. Not used
void onTxDone(void)
{
  
}

//Called when some data is received. Used for acknowledgement
void onReceive(int packetSize)
{
  //Checks if the packet has the exact length for GATEWAY_ID and EMITTER_ID to fit
  if(packetSize == GATEWAY_ID_LEN + 1)
  {
    uint8 in_packet[GATEWAY_ID_LEN + 1];
    int w = 0;

    //Reads the ID values and compares to the established IDs
    for(w = 0;w<(GATEWAY_ID_LEN+1);w++) in_packet[w] = LoRa.read();
    if(in_packet[0] == (((GATEWAY_ID & 0xFF00) >> 8)) and (in_packet[1] == (GATEWAY_ID & 0x00FF)) and (in_packet[2] == EMITTER_ID))
    {
      ack_received = true;
    }
  } 
}
