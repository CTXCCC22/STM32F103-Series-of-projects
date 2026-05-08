#include "stm32f10x.h"

/* 初始化5路红外循迹模块GPIO口（PA8~PA12） */
void Tracker_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;

    //小车在强光下或者没线的地方容易“乱动误判”，说明引脚受到了干扰。
    //模块检测到黑线输出高电平，则改为下拉输入(GPIO_Mode_IPD)，反之改为上拉(GPIO_Mode_IPU)。
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Tracker_Read(void)
{
    //寄存器级同步读取
    //不要调用5次读取函数！
    //小车在高速行驶时，调用5次函数会产生微小的时间差，可能导致“上一微秒读左边还是黑的，下一微秒读中间已经是白的了”，产生逻辑割裂。
    //直接读取GPIOA整个端口的数据，保证这5个传感器是在【同一个系统时钟周期】被拍下“快照”的。
    uint16_t port_val = GPIOA->IDR; 

    //位运算代替函数开销
    //循迹优先走中间，直接通过位与(&)操作提取对应引脚状态。
    //执行效率比调用 GPIO_ReadInputDataBit 提升了几十倍。
    if (port_val & GPIO_Pin_10) return 3; // 中间 (PA10)
    if (port_val & GPIO_Pin_9)  return 2; // 左中 (PA9)
    if (port_val & GPIO_Pin_11) return 4; // 右中 (PA11)
    if (port_val & GPIO_Pin_8)  return 1; // 最左 (PA8)
    if (port_val & GPIO_Pin_12) return 5; // 最右 (PA12)

    return 0; // 都没检测到黑线 → 返回0
}
