#include <SPI.h>
#include <LoRa.h>
#include <esp_sleep.h>

RTC_DATA_ATTR byte out_packet [3] = {69, 0, 0}; 

#define SCK 5     //Clock Signal
#define MISO 19   //Master input Slave output
#define MOSI 27   //Master output Slave input
#define SS 18     //Slave Selection
#define RST 14    //Reset PIN
#define DIO0 26   //Digital Input-Output


int LoRaConfig(int sck, int miso, int mosi, int ss, int rst, int dio0, int freq, int sf, int bw);
void SwReset(int countdown);
void onTxDone(void);

void setup() {
 
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");

  if(LoRaConfig(SCK, MISO, MOSI, SS, RST, DIO0, 868E6, 7, 250E3))
  {
    SwReset(10);
  }
  //LoRa.onTxDone(onTxDone);
  
  delay(1000);

  // send packet
  Serial.print("Sending: ");
  for (int i = 0; i<3;i++)
  {
    Serial.print(out_packet[i]);
    Serial.print(" | ");
  }
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

//Called when the sending of data is finished.
void onTxDone(void)
{
  //sleep for 1'
  Serial.print("Sleeping for 1 minute");
  esp_deep_sleep(60*1000000);
}
