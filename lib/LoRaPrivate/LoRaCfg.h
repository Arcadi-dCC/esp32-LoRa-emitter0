#ifndef LoRaCfg_H
#define LoRaCfg_H

#define SCK         5
#define MISO        19
#define MOSI        27
#define SS          18
#define RST         14
#define DIO0        26

#define FREQ        868E6 //LoRa signal frequency
#define SPR_FACT    7 //LoRa spreading factor
#define BANDWIDTH   250E3 //Lora bandwidth
//#define SYNC_WORD 0x00 //LoRa syncronization byte
//#define PREAM_LEN 8 //LoRa signal preamble length

#define OUT_BUFFER_SIZE     4 //Bytes per packet that will be sent

#define EMITTER_ID          0x00 //ID for each emitter

#define GATEWAY_ID          0x53AC //ID that all emmiters must use to communicate with gateway
#define GATEWAY_ID_LEN      2 //Number of bytes that compose the gateway ID

#define ACK_TIMEOUT         5000 //ms

#endif //LoRaCfg_H
