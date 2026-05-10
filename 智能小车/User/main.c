#include "stm32f10x.h"
#include "bluetooth.h"    
#include "hongwai.h"
#include "HC-SR04.H"
#include <string.h>
#include <stdio.h>
#include "Motor.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Car.h"

uint8_t run_flag = 0;      //0=电机锁定(仅显示)，1=电机允许运行
uint8_t track_result;      
uint8_t key;               
float distance;            
char distStr[20];          
uint8_t bluetooth_data = 'I'; 
uint8_t bluetooth_mode = 0;   //0=红外循迹模式，1=蓝牙控制模式
uint8_t bt_timeout = 0;       //蓝牙通信超时倒计时
char bt_display[16];       
char track_display[16];    

const char* ir_str_table[6] = 
{
    "IR: Stop    ", "IR: S_Left  ", "IR: T_Left  ", 
    "IR: Ahead   ", "IR: T_Right ", "IR: S_Right "
};

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Car_Init();       
    LED_Init();       
    Key_Init();       
    OLED_Init();      
    Tracker_Init();   
    Ultrasound_Init();
    Bluetooth_Init(); 
    
    Car_Stop();       
    
    OLED_ShowString(2, 1, "BT: None      ");
    OLED_ShowString(3, 1, "IR: Stop      "); 
    OLED_ShowString(4, 1, "Motor: STOP   "); 
        
    while (1)
    {
        distance = Test_Distance(); 
        sprintf(distStr, "D:%6.2fcm     ", distance);
        OLED_ShowString(1, 1, distStr);

        track_result = Tracker_Read();
        uint8_t ir_index = (track_result <= 5) ? track_result : 0; 
        sprintf(track_display, "%s", ir_str_table[ir_index]);
        
        //蓝牙接收与超时逻辑
        if(bt_rx_flag == 1)
        {
            bluetooth_data = bt_rx_data; 
            bluetooth_mode = 1;          
            bt_rx_flag = 0;              
            bt_timeout = 10;//重置倒计时
            
            //OLED显示当前收到的字母
            sprintf(bt_display, "BT: %c         ", bluetooth_data);
            OLED_ShowString(2, 1, bt_display);
        }
        else if(bluetooth_mode == 1)
        {
            if(bt_timeout > 0) 
					{
						bt_timeout--;
                    } 
					else 
					{
                        bluetooth_data = 'I';//倒计时归零，强制覆盖指令为 'I'
						OLED_ShowString(2, 1, "BT: None (I)  "); 
                    }
        }
		  
        key = Key_GetNum();
        if (key == 1)
        {
            run_flag = !run_flag; 
            if (run_flag == 0) Car_Stop(); 
            
            bluetooth_mode = 0; 
            OLED_ShowString(2, 1, "BT: None      "); 
            bt_rx_flag = 0;     
            bluetooth_data = 'I'; 
            bt_timeout = 0;
        } 
        
        if(run_flag == 1) OLED_ShowString(4, 1, "Motor: RUN    ");
        else              OLED_ShowString(4, 1, "Motor: STOP   ");
        
        if(bluetooth_mode == 1)
        {
            //在OLED显示蓝牙“想执行”的动作,但不直接执行，等到下面的run_flag判断后再执行对应动作。
            switch(bluetooth_data)
            {
                case 'A': OLED_ShowString(3, 1, "Blue: Ahead "); break;  
                case 'E': OLED_ShowString(3, 1, "Blue: Back  "); break;  
                case 'H': OLED_ShowString(3, 1, "Blue: Left  "); break;  
                case 'B': OLED_ShowString(3, 1, "Blue: Right "); break;
                case 'G': OLED_ShowString(3, 1, "Blue: S_Left"); break;
                case 'C': OLED_ShowString(3, 1, "Blue: S_Righ"); break;
                case 'F': OLED_ShowString(3, 1, "Blue: ULeft"); break; 
                case 'D': OLED_ShowString(3, 1, "Blue: URigh"); break; 
                case 'I': OLED_ShowString(3, 1, "Blue: Stop  "); break;
                default:  OLED_ShowString(3, 1, "Blue: UNKN  "); break;  
            }
            //电机执行
            if (run_flag == 1)
            {
                switch(bluetooth_data)
                {
                    case 'A': Go_Ahead();          break;  
                    case 'E': Go_Back();           break;  
                    case 'H': Turn_Left();         break;  
                    case 'B': Turn_Right();        break;
                    case 'G': Self_Left();         break;
                    case 'C': Self_Right();        break;
                    case 'F': Turn_Around_Left();  break;  
                    case 'D': Turn_Around_Right(); break; 
                    case 'I': Car_Stop();          break;
                    default:  Car_Stop();          break;  
                }
            }
            else Car_Stop();
        }
        else//红外循迹模式
        {
            OLED_ShowString(3, 1, track_display);

            if (run_flag == 1)
            {
                if     (track_result == 3) {Go_Ahead();  }
                else if(track_result == 2) {Turn_Left(); }
                else if(track_result == 4) {Turn_Right();}
                else if(track_result == 1) {Self_Left(); }
                else if(track_result == 5) {Self_Right();}
                else                       {Car_Stop();  }
            }
            else Car_Stop(); 
        }
    }
}

