#include <Arduino.h>

#include <LoRaPrivate.h>

#include <SPI.h>
#include <LoRa.h>
#include <customUtilities.h>

RTC_DATA_ATTR uint8 out_packet [OUT_BUFFER_SIZE] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, EMITTER_ID, 69, 31, 31}; //Final items: data id, data0, data1
volatile uint8 in_packet [IN_BUFFER_SIZE];

volatile bool Cad_isr_responded = false;
volatile bool channel_busy = true;
volatile bool ack_received = false;
volatile bool cldtime_received = false;

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
  //LoRa.setTxPower(TX_POWER);
  //LoRa.setGain(LNA_GAIN);
  //LoRa.setCodingRate4(CR_DEN);
  //LoRa.setPreambleLength(PREAM_LEN);
  //LoRa.enableInvertIQ();
  //LoRa.setSyncWord(SYNC_WORD);
  LoRa.enableCrc();

  LoRa.onCadDone(onCadDone); //call onCadDone ISR when channel activity detection has finished
  //LoRa.onTxDone(onTxDone); //call onTxDone ISR when packet has been fully sent
  LoRa.onReceive(onReceive); //call onReceive ISR when signals are received
  
  return 0;  
}

//Blocking. Checks if someone is using the configured channel. Returns true if used, false if free. 1s timeout
bool isChannelBusy(void)
{
  Cad_isr_responded = false;
  LoRa.channelActivityDetection();
  uint32 start_time = millis();
  while((!Cad_isr_responded) and ((millis() - start_time) < 1000U))
  {
    NOP();
  }
  if(Cad_isr_responded)
  {
    return channel_busy;
  }
  else
  {
    return 1;
  }
}

//ISR called when Channel Activity Detection has finished.
void onCadDone(bool signalDetected) //true menas signal is detected
{
  channel_busy = signalDetected;
  Cad_isr_responded = true;
}

//Prepares the packet with the data to send next. Returns 0 if successful, 1 if error.
uint8 prepareNextPacket(void)
{
  //establish a new random data ID
  uint8 old_data_id = out_packet[GATEWAY_ID_LEN + 1U];
  do
  {
    out_packet[GATEWAY_ID_LEN + 1U] = (uint8)random(0xFF);
  }while(out_packet[GATEWAY_ID_LEN + 1U] == old_data_id);

  //Ascending number 0 -> 32 is sent twice
  out_packet[GATEWAY_ID_LEN + 2U] = (out_packet[GATEWAY_ID_LEN + 3U] + 1U) % 32U;
  out_packet[GATEWAY_ID_LEN + 3U] = (out_packet[GATEWAY_ID_LEN + 3U] + 1U) % 32U;
  
  return 0;
}

//Sends a message to gateway asking for an update on calendar time.
//Returns 0 if the asking is sucessful, 1 if error, 2 if channel is busy.
uint8 askCalendarTime(void)
{
  out_packet[GATEWAY_ID_LEN] = CLDTIME_MSG_ID;
  if(isChannelBusy())
  {
    out_packet[GATEWAY_ID_LEN] = EMITTER_ID;
    return 2U;
  }
  uint8 returner = sendPacket(out_packet, GATEWAY_ID_LEN + 1U);
  out_packet[GATEWAY_ID_LEN] = EMITTER_ID;
  return returner;
}

//Sends a packet through LoRa. Blocking. Returns 0 if successful, 1 if error
uint8 sendPacket(uint8* packet, uint16 packet_len)
{
  Serial.print("Sending: ");
  printStr(packet, packet_len);
  Serial.println();
  while(!LoRa.beginPacket());
  LoRa.write(packet, packet_len);
  LoRa.endPacket(false); //blocking mode

  return 0;
}

//Waits for acknowledgement for some time.
//Blocking, with ACK_TIMEOUT.
//Returns 0 if ACK, 1 if no ACK
uint8 awaitAck(void)
{
  LoRa.receive();
  uint32 start_time = millis();
  while((!ack_received) and ((millis() - start_time) < ACK_TIMEOUT))
  {
    NOP();
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

//Waits for reply with calendar time.
//Blocking, with ACK_TIMEOUT.
//Returns 0 if calendar time was received, 1 if not.
uint8 awaitCalendarTimeReply(time_t* cldtime)
{
  LoRa.receive();
  uint32 start_time = millis();
  while((!cldtime_received) and ((millis() - start_time) < CLDTIME_TIMEOUT))
  {
    NOP();
  }
  *cldtime = 0;
  while(!LoRa.beginPacket()); //exit receive mode
  if(cldtime_received)
  {
    cldtime_received = false;

    (*cldtime) = *((time_t*)(&in_packet[GATEWAY_ID_LEN + 1U])); //little endian arch
    
    Serial.print("Received calendar time string: ");
    printStrHEX((uint8*)in_packet, 7U);
    Serial.println();
    Serial.print("With reconstructed calendar time value: 0x");
    Serial.print(*cldtime, HEX);
    Serial.print(" = DEC ");
    Serial.println(*cldtime);
    return 0;
  }
  else
  {
    return 1;
  }
}

/* NOT USABLE IN EMITTER
//Sends a small packet with GATEWAY_ID and the EMITTER_ID received through LoRa.
//Blocking: Waits for the channel to be available, with ACK_TIMEOUT.
//Returns 0 if successful, 1 if error.
uint8 replyAck(void)
{
  uint8 err_reg = 1;
  while(!LoRa.beginPacket()); //exit receive mode

  uint32 start_time = millis();
  do
  {
    err_reg = (uint8)isChannelBusy();
  } while(err_reg and ((millis() - start_time) < ACK_TIMEOUT));

  if(!err_reg)
  {
    uint8 out_packet[3] = {(GATEWAY_ID & 0xFF00) >> 8, GATEWAY_ID & 0x00FF, in_packet[GATEWAY_ID_LEN]};
    err_reg = sendPacket(out_packet, sizeof(out_packet));
  }

  LoRa.receive(); //reenter receive mode
  return err_reg;
}*/

/* NOT USED
//ISR called when the sending of data is finished.
void onTxDone(void)
{
  
}*/

//ISR called when detecting LoRa signals in receive mode. Used for acknowledgement
void onReceive(int packetSize)
{
  uint8 w = 0;
  switch (packetSize)
  {
    case (GATEWAY_ID_LEN + 1U): //Exact fit for ACK message
    {    
      //Reads the ID values and compares to the established IDs
      for(w = 0;w<(GATEWAY_ID_LEN+1);w++) in_packet[w] = LoRa.read();
      if(in_packet[0] == (((GATEWAY_ID & 0xFF00) >> 8)) and (in_packet[1] == (GATEWAY_ID & 0x00FF)) and (in_packet[2] == EMITTER_ID))
      {
        ack_received = true;
      }
      break;
    }

    case (GATEWAY_ID_LEN + 5U): //Exact fit for calendar time message
    {
      //Reads the ID values and compares to the estabished GATEWAY_ID and CLDTIME_MSG_ID
      for(w = 0;w<(GATEWAY_ID_LEN+5);w++) in_packet[w] = LoRa.read();
      if(in_packet[0] == (((GATEWAY_ID & 0xFF00) >> 8)) and (in_packet[1] == (GATEWAY_ID & 0x00FF)) and (in_packet[2] == CLDTIME_MSG_ID))
      {
        cldtime_received = true;
      }
      break;
    }

    default:
    {
      //Do nothing
    }
  }
}
