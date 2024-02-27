#ifndef LoRaPrivate_H
#define LoRaPrivate_H

#include <platformTypes.h>
#include <LoRaCfg.h>

uint8 LoRaConfig(void);
bool isChannelBusy(void);
uint8 sendPacket(uint8* packet, uint16 packet_len);
uint8 awaitAck(void);
//uint8 replyAck(void);
void onCadDone(bool signalDetected);
//void onTxDone(void);
void onReceive(int packetSize);

extern RTC_DATA_ATTR uint8 out_packet [OUT_BUFFER_SIZE];

#endif //LoRaPrivate_H
