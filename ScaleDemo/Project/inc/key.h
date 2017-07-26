#ifndef __KEY_H__
#define __KEY_H__
#include "stm8s.h"

#define KEY_NUM              16       
//2ms 扫描一次按键
#define KEY_PRESS_TIME       10       //100ms
#define KEY_PRESS_TIME_3S    300      //3s

//按键识别码
//按键状态标志  2ms
#define KEY_PRESSED          0x1000
#define KEY_PRESSED_3S       0x2000

#define KEY_NULL          0

#define KEY_TARE       0x05
#define KEY_ZERO       0x06
#define KEY_CLEAR      0x07
#define KEY_STAR       0x08

#define KEY_7          0x09
#define KEY_4          0x0A
#define KEY_1          0x0B
#define KEY_0          0x0C

#define KEY_8          0x0D
#define KEY_5          0x0E
#define KEY_2          0x0F
#define KEY_DOT        0x10

#define KEY_9          0x11
#define KEY_6          0x12
#define KEY_3          0x13
#define KEY_TOTAL      0x14

#define KEY_RSV1       0xff
#define KEY_RSV2       0xff
#define KEY_RSV3       0xff


void Key_Init(void);
 
void Key_Scan(void);
u16 Key_GetCode(void);
#endif