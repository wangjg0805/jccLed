#ifndef __KEY_H__
#define __KEY_H__
#include "stm8s.h"

#define KEY_NUM                          6       //6个按键
//2ms 扫描一次按键
#define KEY_PRESS_TIME                  25       //按键多少时间被认可
#define CLICK_TIME                     150       //单击按键最大时间允许
#define KEY_3S_TIME                   1500       //按键3S

//按键识别码
//原来的
#define KEY_ESC           4
#define KEY_SAVE          5
#define KEY_MODE          0
#define KEY_UP            2
#define KEY_DOWN          1
#define KEY_ENTER         3
/*/后来更改的 
#define KEY_ESC           1
#define KEY_SAVE          0
#define KEY_MODE          5
#define KEY_UP            4
#define KEY_DOWN          3
#define KEY_ENTER         2
*/

//按键状态标志  2ms*
#define KEY_PRESSED          0x10
#define KEY_PRESSED_3S       0x20
//声音区

#define BEEP_PIN       GPIO_PIN_4

 void Beep_Start(void);
 void Beep_Stop(void);
 void Beep_Pin_Init(void);
 void Key_Pin_Init(void);
 
 void Key_Scan(void);
 u8 Get_Key_Code(void);
#endif