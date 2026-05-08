#include "stm32f10x.h"                  // Device header
#include "Delay.h"

float AAA = 0.0f; 

void Ultrasound_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Trig引脚 PB14 推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Echo引脚 PB13 下拉输入 (防止悬空时的误触发)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化完成后，确保Trig引脚拉低，避免上电误触发
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    
    TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1; // 60ms的最大量程时间
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // 1us计数一次
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}

// 单次获取原始距离数据 (带有超时防卡死机制)
float Get_Raw_Distance(void)
{
    uint16_t timeout;
    
    // 1. 发送10us以上的高电平触发信号
    GPIO_SetBits(GPIOB, GPIO_Pin_14); 
    Delay_us(15); // 稍微给多一点点，保证触发
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    // 2. 等待Echo变高 (加入超时判定，防止卡死)
    timeout = 10000; 
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        timeout--;
        if(timeout == 0) return -1.0f; // 传感器无响应，返回错误代码
    }
    
    // 3. 开启定时器前先清零
    TIM_SetCounter(TIM4, 0); 
    TIM_Cmd(TIM4, ENABLE);
    
    // 4. 等待Echo变低 (靠定时器自身的值来做超时防卡死)
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET)
    {
        // 超过50000us (50ms，相当于8米多)，说明超出量程或出错
        if(TIM_GetCounter(TIM4) > 50000) 
        {
            TIM_Cmd(TIM4, DISABLE);
            return -1.0f; 
        }
    }
    
    // 5. 停止定时器并计算
    TIM_Cmd(TIM4, DISABLE);
    uint16_t count = TIM_GetCounter(TIM4);
    
    // 距离 = 时间(us) * 0.034 / 2 = 时间 * 0.017
    return (count * 0.017f); 
}

//供外部调用的测距函数：中位值平均滤波法
//连续测3次，如果嫌不够稳可以改为测5次掐头去尾
float Test_Distance(void)
{
    float dis[3];
    float temp;
    int valid_cnt = 0;
    
    //连续采集3次有效数据
    for(int i = 0; i < 3; i++)
    {
        temp = Get_Raw_Distance();
        if(temp > 0) // 数据有效
        {
            dis[valid_cnt] = temp;
            valid_cnt++;
        }
        //每次发射后必须延时等待声波消失，否则下次必受干扰！
        Delay_ms(20); 
    }
    
    //如果一次有效数据都没采到，返回上一次的值
    if(valid_cnt == 0) return AAA; 
    
    //简单的排序法 (从小到大)
    for(int i = 0; i < valid_cnt - 1; i++) 
	{
        for(int j = i + 1; j < valid_cnt; j++) 
		{
            if(dis[i] > dis[j]) 
			{
                float t = dis[i];
                dis[i] = dis[j];
                dis[j] = t;
            }
        }
    }
    
    //根据有效采样的次数决定如何取值
    if(valid_cnt == 3) 
	{
        AAA = dis[1]; //采到3个，取中间的那个（丢弃最大和最小值，完美过滤突变干扰）
    } 
	else if(valid_cnt == 2) 
	{
        AAA = (dis[0] + dis[1]) / 2.0f; //采到2个，取平均
    } 
	else 
	{
        AAA = dis[0]; //只采到1个
    }
    
    return AAA;
}
