#ifndef LoRaCfg_H
#define LoRaCfg_H

//SPI pins for interaction with LoRa peripheral unit.
#define SCK         5
#define MISO        19
#define MOSI        27
#define SS          18
#define RST         23
#define DIO0        26

#define FREQ        868E6                           //Signal frequency
#define SPR_FACT    12                              //Spreading factor
#define BANDWIDTH   125E3                           //Bandwidth
#define TX_POWER    20                              //Transmitter power [2, 20] (dB)
#define LNA_GAIN    0                               //Low-noise amplifier gain [1, 6]. 0->automatic
#define CR_DEN      8                               //Denominator of the coding rate [5, 8]. (CR = 4/x)
//#define SYNC_WORD 0x00                            //Syncronization byte
//#define PREAM_LEN 8                               //Signal preamble length

#define EMITTER_ID          0x00                    //ID for each emitter

#define GATEWAY_ID          0x53AC                  //ID that all emmiters must use to communicate with gateway
#define GATEWAY_ID_LEN      2U                      //Number of bytes that compose the gateway ID
#define CLDTIME_MSG_ID      0xFF                    //ID that defines a time update message

#define OUT_BUFFER_SIZE     GATEWAY_ID_LEN + 3U     //Bytes per packet that will be sent
#define IN_BUFFER_SIZE      255U                    //Maximum number of bytes per input packet

#define ACK_TIMEOUT         10000U                   //Acknowledgment timeout (ms)
#define CLDTIME_TIMEOUT     10000U                   //Calendar time reply timeout (ms)

#endif //LoRaCfg_H
