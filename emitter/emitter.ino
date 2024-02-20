#include <SPI.h>
#include <LoRa.h>
#include <esp_sleep.h>

#define SCK 5     //Clock Signal
#define MISO 19   //Master input Slave output
#define MOSI 27   //Master output Slave input
#define SS 18     //Slave Selection
#define RST 14    //Reset PIN
#define DIO0 26   //Digital Input-Output

#define NET_ID 0x53AC //ID that all emmiters must use to communicate with gateway
#define EMITTER_ID 0x00 //ID of the emitter

RTC_DATA_ATTR byte out_packet [4] = {(NET_ID & 0xFF00) >> 8, NET_ID & 0x00FF, EMITTER_ID, 0}; 
boolean channel_busy = true;
boolean Cad_isr_responded = false;
boolean ack_received = false;

int LoRaConfig(int sck, int miso, int mosi, int ss, int rst, int dio0, int freq, int sf, int bw);
void SwReset(int countdown);
boolean isChannelBusy(void);
void onCadDone(boolean signalDetected);
void printStr(byte* str, int len);
//void onTxDone(void);
void onReceive(int packetSize);

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
  if(acknowledgement(5))
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
}

void loop() {
  //unreachable
}




/********************************************************/
/****************  FUNCTION DEFINITIONS  ****************/
/********************************************************/

//Encapsules the whole LoRa configuration. Returns 0 if successful, 1 if error.
int LoRaConfig(int sck, int miso, int mosi, int ss, int rst, int dio0, int freq, int sf, int bw)
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
boolean isChannelBusy(void)
{
  Cad_isr_responded = false;
  LoRa.channelActivityDetection();
  while(!Cad_isr_responded)
  {
    delay(1);
  }
  return channel_busy;
}

void onCadDone(boolean signalDetected) //true = signal detected
{
  channel_busy = signalDetected;
  Cad_isr_responded = true;
}

void printStr(byte* str, int len)
{
  Serial.print("| ");
  for (int w = 0; w < len; w++)
  {
    Serial.print(str[w]);
    Serial.print(" | ");
  }
}

//Performs a software reset after warning through the serial output and doing a countdown.
void SwReset(int countdown)
{
  Serial.print("Reseting the MCU in ");
  for (int w=countdown;w>0;w--)
  {
    Serial.print(w);
    Serial.print(" ");
    delay(1000);
  }
  ESP.restart();
}

//Waits for acknowledgement for some time. Timeout in ms. Returns 0 if ACK, 1 if no ACK
int acknowledgement(unsigned long timeout)
{
  unsigned long start_time = millis();
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
  int w = 0;
  //Checks if the packet is long enough for an ID to fit
  if(packetSize > 2)
  {
    //Reads the ID values and compares to the established ID
    for(w = 0;w<2;w++) values[w] = LoRa.read();
    if(values[0] == (((NET_ID & 0xFF00) >> 8)) and (values[1] == (NET_ID & 0x00FF)))
    {
      //Continues if ID is correct
      if(packetSize > BUFFER_SIZE) num_values = BUFFER_SIZE;
      else num_values = packetSize;
      
      for(w = 2; w < num_values; w++) values[w] = LoRa.read();
    }
  } 
}
