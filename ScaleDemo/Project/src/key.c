#include "stm8s.h"
#include "global.h"
#include "key.h"

static volatile u8 key_status,key_press,key_press_3s;    //每一位对应一个按键(为1表示对应事件发生)

////////////////////////////
//蜂鸣器管脚初始化
///////////////////////////
void Beep_Start(void)
 {
   GPIO_WriteLow(GPIOD, BEEP_PIN); 
 }
void Beep_Stop(void)
 {
   GPIO_WriteHigh(GPIOD, BEEP_PIN); 
 }

void Beep_Pin_Init(void)
 {
   GPIO_Init(GPIOD,BEEP_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
   Beep_Stop(); 
 }

////////////////////////////
//键盘管脚初始化
///////////////////////////
void Key_Pin_Init(void)
 {  //6个按键
   GPIO_Init(GPIOC,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
   GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);
 }
//**********************************************************************
//函数  名:Key_Scan
//函数功能:
//入口参数:无
//出口参数:无
//说    明:每2ms扫描一次
//**********************************************************************
#define KEY_MASK    0x11
void  Key_Scan(void)
    {
      static u16 key_press_time[KEY_NUM];    //记录每个按键按下的时间
              u8 i,j;
  
      key_status = GPIO_ReadInputData(GPIOC)|KEY_MASK; //某位为0表示有按键
      if(0xff == key_status)
      {
         for (i=0; i<KEY_NUM; i++)
           key_press_time[i] = 0;  //清零
         
         key_press = 0;
      }  
      else
      { //有键按下
         for (i=0; i<KEY_NUM; i++) //扫描每个按键信号
         {  
            j = 1;        //依次得到屏蔽位
            switch(i)
            {
             case 0: j=1;break;
             case 1: j=2;break;
             case 2: j=3;break;
             case 3: j=5;break;
             case 4: j=6;break;
             case 5: j=7;break;
             default:break;
            }
             if(key_status & (1 << j))
             {  //某键未按下,累加时间清零
                key_press_time[i] = 0;
             }
             if(key_press_time[i] == KEY_PRESS_TIME)
             {  //某键按了下去(保持50ms)
                key_press |= 1 << i;
             }      
             if(key_press_time[i]== KEY_3S_TIME)
             {  //某键一直按了3s
                key_press_3s |= 1 << i;
             }
             key_press_time[i]++;   //当前按键时间增加           
          }
      }
   }
//////////////////////////////////
//**********************************************************************
//函数  名:Get_Key_Status
//函数功能:读键状态
//入口参数:无
//出口参数:键状态值(对应位1表示此键被按下)
//**********************************************************************
u8 Get_Key_Status(void)
  {
    return(key_status);
  }

/****************************************************************************
* 名称：Get_KeyCode()
* 功能：读键码
* 入口参数：无
* 出口参数：低4位键码(0~15), 高4位动作(0 未按,1 按下,2 按下2s, 3 按下5s)
****************************************************************************/
u8 Get_Key_Code(void)
{
    u8 i;
    for(i=0; i<KEY_NUM; i++)
    {    
       if(key_press & (1 << i))
        {
            key_press = 0;        //清键值
            return(KEY_PRESSED + i);
        }     

       if(key_press_3s & (1 << i))
        {
          if(0xff==key_status)//释放按键后清键值
            key_press_3s = 0;    //清键值 
            return(KEY_PRESSED_3S + i);
        }
    }
    return(0);
}

