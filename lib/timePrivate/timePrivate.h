#ifndef timePrivate_H
#define timePrivate_H

#include <platformTypes.h>

#include <ESP32Time.h>

void sleepFor(uint16 seconds, uint16 minutes = 0U, uint16 hours = 0U, uint16 days = 0U);
uint8 sleepUntil(int year, int month, int day, int hour, int minute, int second);

extern ESP32Time rtc;

#endif //timePrivate_H
