#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm8s.h"


void LedCpu_Init(void);
void LedCpu_Reverse(void);
void LedCpu_On(void);
void LedCpu_Off(void);


////////////////////////////
//·äÃùÆ÷¹Ü½Å³õÊ¼»¯
///////////////////////////
void BkLight_Reverse(void);
void BkLight_On(void);
void BkLight_Off(void);

#endif