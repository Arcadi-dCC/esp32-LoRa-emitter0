#ifndef LoRaPrivate_H
#define LoRaPrivate_H

#include <platformTypes.h>

uint8 LoRaConfig(void);
bool isChannelBusy(void);
void onCadDone(bool signalDetected);
uint8 sendPacket(uint8* packet, uint16 packet_len);
void onTxDone(void);
void onReceive(int packetSize);
uint8 acknowledgement(uint16 timeout);

#endif //LoRaPrivate_H
