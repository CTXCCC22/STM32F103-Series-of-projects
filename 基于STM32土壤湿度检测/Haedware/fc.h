#ifndef __FC_H
#define __FC_H

#include "stm32f10x.h"

void FC_Init(void);
uint16_t FC_GetADCValue(void);
uint8_t FC_GetMoisturePercent(void);
uint8_t FC_GetDOStatus(void);

#endif
