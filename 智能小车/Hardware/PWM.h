#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void PWM_Init(void);
void PWM_SetCompare3(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
void PWM_SetDutyCycle2(uint8_t percent);
void PWM_SetDutyCycle3(uint8_t percent);

#endif
