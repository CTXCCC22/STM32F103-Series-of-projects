#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f10x.h"

/* 声明外部变量，让 main.c 能直接读取蓝牙接收到的数据和标志位 */
extern volatile uint8_t bt_rx_data;
extern volatile uint8_t bt_rx_flag;

/* 蓝牙初始化函数声明 */
void Bluetooth_Init(void);

#endif /* __BLUETOOTH_H */
