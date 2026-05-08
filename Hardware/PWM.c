#include "stm32f10x.h"
#include "PWM.h"

/* 初始化TIM2双通道PWM（PA1=CH2，PA2=CH3）*/
void PWM_Init(void)
{
	/*开启时钟：TIM2 + GPIOA*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*GPIO 初始化：PA1、PA2 复用推挽输出*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*定时器时基配置：72MHz/36/100 = 20kHz PWM频率*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;          //自动重装载值，决定PWM周期，这里设置为100-1，即计数0~99，周期100个计数
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;       //预分频值，决定计数频率，这里设置为36-1，即分频系数36，使得计数频率为72MHz/36=2MHz，每0.5us计数一次
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	/*PWM输出比较配置*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;

	/*配置通道2（PA1）*/
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	/*配置通道3（PA2）*/
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	/*ARR预装载使能 + 启动定时器 */
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

/*设置TIM2_CH3（PA2）的比较值CCR3*/
void PWM_SetCompare3(uint16_t Compare)
{
	if (Compare > 99) 
	{
		Compare = 99;
	}
	TIM_SetCompare3(TIM2, Compare);
}

/* 设置TIM2_CH2（PA1）的比较值CCR2*/
void PWM_SetCompare2(uint16_t Compare)
{
	if (Compare > 99) 
	{
		Compare = 99;
	}
	TIM_SetCompare2(TIM2, Compare);
}

/*下面两个函数只是用于设置占空比的辅助函数，防止重复代码，确保占空比以百分比形式表示*/

//按百分比设置CH2（PA1）占空比
void PWM_SetDutyCycle2(uint8_t  percent)
{
	if (percent > 100)
		{
			percent = 100;
		}
	if (percent < 0)
		{
			percent = 0;
		}
	
	uint16_t compare = percent;
	TIM_SetCompare2(TIM2, compare);
}

//按百分比设置CH3（PA2）占空比
void PWM_SetDutyCycle3(uint8_t percent)
{
	if (percent > 100)
		{
			percent = 100;
		}
	if (percent < 0)
		{
			percent = 0;
		}
	uint16_t compare = percent;
	TIM_SetCompare3(TIM2, compare);
}
