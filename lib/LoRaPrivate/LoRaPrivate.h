#ifndef LoRaPrivate_H
#define LoRaPrivate_H

#include <platformTypes.h>
#include <LoRaCfg.h>

uint8 LoRaConfig(void);
bool isChannelBusy(void);
void onCadDone(bool signalDetected);
uint8 sendPacket(uint8* packet, uint16 packet_len);
void onTxDone(void);
void onReceive(int packetSize);
uint8 awaitAck(void);
//uint8 replyAck(void);

extern RTC_DATA_ATTR uint8 out_packet [OUT_BUFFER_SIZE];

#endif //LoRaPrivate_H
