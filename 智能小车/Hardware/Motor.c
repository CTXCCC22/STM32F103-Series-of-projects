#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	PWM_Init();
}

void Motor_SetLeftSpeed(int8_t Speed)
{
	if (Speed > 0)//正转
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);  //PA4置高电平
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);//PA5置低电平
		PWM_SetCompare3(Speed);
	}
	else if(Speed < 0)//反转
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);//PA4置低电平
		GPIO_SetBits(GPIOA, GPIO_Pin_5);  //PA5置高电平
		PWM_SetCompare3(-Speed); 
	}
	else//停止
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);//PA4置低电平
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);//PA5置低电平
		PWM_SetCompare3(0); 
	}		
}

void Motor_SetRightSpeed(int8_t Speed)
{
	if (Speed > 0)//正转
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);  //PA置高电平
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);//PA置低电平
		PWM_SetCompare2(Speed);
	}
	else if(Speed < 0)//反转
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);//PA置低电平
		GPIO_SetBits(GPIOA, GPIO_Pin_7);  //PA置高电平
		PWM_SetCompare2(-Speed);
	}
	else//停止
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);//PA置低电平
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);//PA置低电平
		PWM_SetCompare2(0);
	}
}
