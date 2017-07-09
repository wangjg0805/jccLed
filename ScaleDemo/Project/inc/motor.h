#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm8s.h"

#define  MOTOR_ARRIVE  1
#define  MOTOR_NO_ARR  0

#define  LAMP_DAO      1
#define  LAMP_WU       0

#define PHASE_A_L   GPIO_WriteLow(GPIOB,   GPIO_PIN_0)
#define PHASE_B_L   GPIO_WriteLow(GPIOB,   GPIO_PIN_1)
#define PHASE_C_L   GPIO_WriteLow(GPIOB,   GPIO_PIN_2)
#define PHASE_D_L   GPIO_WriteLow(GPIOB,   GPIO_PIN_3)

#define PHASE_A_H   GPIO_WriteHigh(GPIOB,  GPIO_PIN_0)
#define PHASE_B_H   GPIO_WriteHigh(GPIOB,  GPIO_PIN_1)
#define PHASE_C_H   GPIO_WriteHigh(GPIOB,  GPIO_PIN_2)
#define PHASE_D_H   GPIO_WriteHigh(GPIOB,  GPIO_PIN_3)

void Motor_Pin_Init(void);
void motor_fan_run(u16 steps);

#endif