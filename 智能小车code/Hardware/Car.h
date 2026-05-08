#ifndef __CAR_H
#define __CAR_H

#include "stm32f10x.h"
#include "Motor.h"

void Car_Init(void);
void Go_Ahead(void);
void Go_Back(void);
void Turn_Left(void);
void Turn_Right(void);
void Self_Left(void);
void Self_Right(void);
void Car_Stop(void);
void Turn_Around_Left(void);
void Turn_Around_Right(void);

#endif
