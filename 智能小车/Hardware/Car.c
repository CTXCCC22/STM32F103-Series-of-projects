#include "stm32f10x.h"
#include "Car.h"
#include "Motor.h"
#include "Delay.h"

// 小车初始化
void Car_Init(void)
{
	Motor_Init();
}

// 小车全速前进
void Go_Ahead(void)
{
	Motor_SetLeftSpeed(99);
	Motor_SetRightSpeed(-99);
}

// 小车后退
void Go_Back(void)
{
	Motor_SetLeftSpeed(-99);
	Motor_SetRightSpeed(99);
}

// 小车停止
void Car_Stop(void)
{
	Motor_SetLeftSpeed(0);
	Motor_SetRightSpeed(0);
}

// 小车左微调（循迹用）
void Turn_Left(void)
{
	Motor_SetLeftSpeed(40);
	Motor_SetRightSpeed(-99);
}

// 小车右微调（循迹用）
void Turn_Right(void)
{
	Motor_SetLeftSpeed(99);
	Motor_SetRightSpeed(-40);
}

// 小车大左转 / 原地左转
void Self_Left(void)
{
	Motor_SetLeftSpeed(0);
	Motor_SetRightSpeed(-99);
}

// 小车大右转 / 原地右转
void Self_Right(void)
{
	Motor_SetLeftSpeed(99);
	Motor_SetRightSpeed(0);
}

//向左掉头
void Turn_Around_Left(void)
{
	Motor_SetLeftSpeed(-99);
	Motor_SetRightSpeed(-99);
}

//向右掉头
void Turn_Around_Right(void)
{
	Motor_SetLeftSpeed(99);
	Motor_SetRightSpeed(99);
}
// 功能：小车行进方向控制
