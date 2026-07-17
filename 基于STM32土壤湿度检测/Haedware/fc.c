#include "fc.h"
#include "Delay.h"

void FC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//将PA0配置为模拟输入模式，用于读取AO引脚的模拟电压
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//将PA1配置为浮空输入模式，用于读取DO引脚状态

    /* 配置ADC1 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC时钟12MHz，PCLK2=72MHz/6=12MHz
    ADC_DeInit(ADC1);                //复位ADC1寄存器为默认值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  //独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       //单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 //单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             //转换通道数为1
    ADC_Init(ADC1, &ADC_InitStructure);

    /*设置ADC1通道0（PA0）为规则通道，采样时间55.5周期 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    //复位校准寄存器,目的是清除ADC的校准寄存器，确保ADC在开始使用前处于已知状态
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    //ADC校准,目的是提高ADC的精度和稳定性，确保ADC在转换模拟信号时能够提供准确的数字输出
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
}

//此函数仅用于获取ADC值，返回值范围0~4095
uint16_t FC_GetADCValue(void)
{
    uint16_t adcValue;

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);                //启动ADC软件转换
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);//等待转换完成
    adcValue = ADC_GetConversionValue(ADC1);               //读取ADC转换值 
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);                     //清除转换完成标志

    return adcValue;
}

//冒泡排序（小→大），用于中值滤波
static void BubbleSort(uint16_t *arr, uint8_t n)
{
    uint8_t i, j;
    uint16_t temp;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

//将ADC值转换为湿度百分比（整数0~100），然后把湿度百分比返回给主程序
uint8_t FC_GetMoisturePercent(void)
{
    uint16_t adcValue;
    uint8_t  moisture;
    uint16_t samples[20];     //20个样本用于中值+均值滤波
    uint32_t sum;
    uint8_t  i;
    //采集20个ADC样本
    for (i = 0; i < 20; i++)
    {
        samples[i] = FC_GetADCValue();
        Delay_us(200);        //采样间隔200us，避免连续采样值相关性过高 
    }

    //中值滤波：排序后剔除最大和最小的各5个极端值，取中间10个求平均
    BubbleSort(samples, 20);

    sum = 0;
    for (i = 5; i < 15; i++)  //取samples[5]~samples[14]，共10个中间值 */
    {
        sum += samples[i];
    }
    adcValue = (uint16_t)(sum / 10);

    //有效检测判断：ADC值过高(>3800)说明传感器未插入土壤（开路），返回0% 
    //传感器悬空时AO引脚为高电平，ADC接近4095，不属于有效土壤读数 
    if (adcValue > 3800)
    {
        return 0;
    }

    //将ADC值转换为湿度百分比（整数0~100）
    //使用四舍五入提高精度：(adcValue * 100 + 1900) / 3800
    //ADC有效范围0~3800，湿度0%~100%（排除开路无效区间）
    //土壤越湿，AO输出电压越高，ADC值越大 → 湿度百分比越大 
    moisture = (uint8_t)(((uint32_t)adcValue * 100 + 1900) / 3800);

    //限制在0~100范围内
    if (moisture > 100)
        moisture = 100;

    return moisture;
}

//此函数用于获取DO引脚状态
uint8_t FC_GetDOStatus(void)
{
    //读取PA1引脚电平（DO数字输出）
    //返回0表示高于阈值（土壤干燥），返回1表示低于阈值（土壤湿润）
    return (uint8_t)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}
