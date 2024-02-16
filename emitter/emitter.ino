#include <SPI.h>
#include <LoRa.h>
#include <esp_sleep.h>

#define SCK 5     //Clock Signal
#define MISO 19   //Master input Slave output
#define MOSI 27   //Master output Slave input
#define SS 18     //Slave Selection
#define RST 14    //Reset PIN
#define DIO0 26   //Digital Input-Output

#define NET_ID 0x53AC ////ID that all emmiters must use to communicate with gateway

RTC_DATA_ATTR byte out_packet [3] = {(NET_ID & 0xFF00) >> 8, NET_ID & 0x00FF, 0}; 
boolean channel_busy = true;
boolean Cad_isr_responded = false;

int LoRaConfig(int sck, int miso, int mosi, int ss, int rst, int dio0, int freq, int sf, int bw);
void SwReset(int countdown);
boolean isChannelBusy(void);
void onCadDone(boolean signalDetected);
void printStr(byte* str, int len);
void onTxDone(void);

void setup() {
 
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  if(LoRaConfig(SCK, MISO, MOSI, SS, RST, DIO0, 868E6, 7, 250E3))
  {
    SwReset(10);
  }
  LoRa.onCadDone(onCadDone);
  //LoRa.onTxDone(onTxDone);
  
  delay(1000);

  //Retry in a few seconds if channel is busy
  if(isChannelBusy())
  {
    Serial.println("Channel is busy. Retrying in a few seconds");
    esp_deep_sleep(random(10,14)*1000000);
  }

  // send packet
  Serial.print("Sending: ");
  printStr(out_packet, sizeof(out_packet));
  Serial.println();
  while(!LoRa.beginPacket());
  //LoRa.print("0");                //Configurat per enviar 0 sempre (per no anar variant el tamany dels paquets)
  LoRa.write(out_packet, sizeof(out_packet));
  LoRa.endPacket(false);             //blocking mode
  out_packet[2] = (out_packet[2] + 1 ) % 32;
  Serial.print("Sleeping for 2 minutes");
  esp_deep_sleep(120*1000000);
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

//Called when the sending of data is finished. Currently not used
void onTxDone(void)
{
  //sleep for 1'
  Serial.print("Sleeping for 1 minute");
  esp_deep_sleep(60*1000000);
}
