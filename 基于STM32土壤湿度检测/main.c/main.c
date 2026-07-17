#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "fc.h"
#include "key.h"

//状态机定义
typedef enum 
{
    STATE_IDLE = 0,       //待机状态：不测量，显示"Status: OFF"      
    STATE_MEASURING,      //测量状态：采集ADC并显示湿度百分比       
    STATE_COUNT           //状态总数（用于边界检查）    
} SystemState_t;

//事件定义,用于状态机调度
typedef enum 
{
    EVENT_KEY_PRESSED = 0,//按键按下事件                    
    EVENT_NONE            //无事件                            
} SystemEvent_t;

int main(void)
{
    uint8_t moisture;
    SystemState_t state = STATE_IDLE; //新的枚举类新建一个变量state，初始状态为STATE_IDLE，待机

    OLED_Init();
    FC_Init();
    Key_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Soil Moisture");
    OLED_ShowString(3, 1, "Status: OFF");

    while (1)
    {
        //检测事件
        SystemEvent_t event = Key_IsPressed() ? EVENT_KEY_PRESSED : EVENT_NONE;

        //状态机调度
        switch (state)
        {
            //只要是STATE_IDLE状态，就会执行下面的代码
            case STATE_IDLE:
                // 待机状态动作：清空读数区域
                OLED_ShowString(1, 1, "Soil Moisture");
                OLED_ShowString(2, 1, "             ");
                if (event == EVENT_KEY_PRESSED)//状态转换：按键 → 进入测量 
                {
                    state = STATE_MEASURING;//告诉外面的检测事件，准备下一循环在什么状态下执行
                    OLED_ShowString(3, 1, "Status: ON ");
                }
                break;
//=========================================================================
            case STATE_MEASURING: 
                moisture = FC_GetMoisturePercent();//获取值
                OLED_ShowString(1, 1, "Soil Moisture");
                OLED_ShowString(2, 1, "Moisture:");
                OLED_ShowNum(2, 10, moisture, 3);
                OLED_ShowString(2, 13, "% ");
                if (event == EVENT_KEY_PRESSED)//状态转换：按键 → 回到待机
                {
                    state = STATE_IDLE;//告诉外面的检测事件，准备下一循环在什么状态下执行
                    OLED_ShowString(3, 1, "Status: OFF");
                    break;
                }
                break;
//=========================================================================
            default:
                state = STATE_IDLE;
                break;
        }

        Delay_ms(100);  /* 主循环周期100ms，兼顾按键响应和显示刷新 */
    }
}
