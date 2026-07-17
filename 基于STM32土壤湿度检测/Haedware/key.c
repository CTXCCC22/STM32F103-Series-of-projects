#include "key.h"
#include "Delay.h"

/* 按键GPIO引脚定义：PA11，外接GND，按下为低电平 */
#define KEY_PORT     GPIOA
#define KEY_PIN      GPIO_Pin_11

/**
  * @brief  按键GPIO初始化，将PA11配置为上拉输入模式
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

/**
  * @brief  读取按键状态（已消抖）
  * @param  无
  * @retval 1 表示检测到一次稳定按下
  * @retval 0 表示未按下
  */
uint8_t Key_IsPressed(void)
{
    static uint8_t lastState = 1;  /* 上次稳定状态（1=未按下） */
    uint8_t now;

    if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == 0)
    {
        /* 检测到低电平，延时消抖后再次确认 */
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == 0)
        {
            now = 0;  /* 确认按下 */
        }
        else
        {
            now = 1;  /* 干扰，视为未按下 */
        }
    }
    else
    {
        now = 1;      /* 高电平，未按下 */
    }

    /* 下降沿检测：上次高(未按下) → 现在低(按下) */
    if (lastState == 1 && now == 0)
    {
        lastState = now;
        return 1;    /* 检测到一次有效按键 */
    }
    lastState = now;
    return 0;
}
