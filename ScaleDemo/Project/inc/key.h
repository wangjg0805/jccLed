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

#define KEY_NULL     0
#define KEY_1        1
#define KEY_2        2
#define KEY_3        3
#define KEY_4        4
#define KEY_5        5
#define KEY_6        6
#define KEY_7        7
#define KEY_8        8
#define KEY_9        9
#define KEY_10       10
#define KEY_11       11
#define KEY_12       12
#define KEY_13       13
#define KEY_14       14
#define KEY_15       15
#define KEY_16       16


void Key_Init(void);
 
void Key_Scan(void);
u16 Key_GetCode(void);
#endif