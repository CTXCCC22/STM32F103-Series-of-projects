#include "stm32f10x.h"

void LED_Init(void)
{
    /* 开启时钟：GPIOC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* GPIO 初始化：PC13 推挽输出 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //初始状态：LED全灭（PB5~PB8置高电平）
    GPIO_ResetBits(GPIOB, GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8);
}
