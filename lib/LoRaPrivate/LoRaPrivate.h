#ifndef LoRaPrivate_H
#define LoRaPrivate_H

#include <platformTypes.h>

#define GATEWAY_ID 0x53AC //ID that all emmiters must use to communicate with gateway
#define EMITTER_ID 0x00

uint8 LoRaConfig(uint8 sck,uint8 miso, uint8 mosi, uint8 ss, uint8 rst, uint8 dio0, uint32 freq, uint8 sf, uint32 bw);
bool isChannelBusy(void);
void onCadDone(bool signalDetected);
void onTxDone(void);
void onReceive(int packetSize);
uint8 acknowledgement(uint16 timeout);

extern bool Cad_isr_responded;
extern bool channel_busy;
extern bool ack_received;

#endif //LoRaPrivate_H
